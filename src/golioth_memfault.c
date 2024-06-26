/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_memfault, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>

#include <golioth/client.h>
#include <golioth/rpc.h>
#include <golioth/stream.h>
#include <memfault/components.h>

#include "golioth_memfault.h"

#define TIMER_PERIOD    K_SECONDS(60*60) // 1 hour

struct memfault_work_ctx
{
    struct k_work work;
    struct golioth_client *client;
};

static struct memfault_work_ctx mflt_ctx;

static void memfault_work_handler(struct k_work *work)
{
    struct memfault_work_ctx *ctx = CONTAINER_OF(work, struct memfault_work_ctx, work);
    golioth_memfault_upload(ctx->client);
}

static void mflt_upload_timer_expired(struct k_timer *timer)
{
    /* This function is executed in an interrupt context, so all we do
       here is schedule work on the system work queue */
    struct memfault_work_ctx *ctx = k_timer_user_data_get(timer);
    k_work_submit(&ctx->work);
}
K_TIMER_DEFINE(mflt_upload_timer, mflt_upload_timer_expired, NULL);

static void coredump_timer_expired(struct k_timer *timer)
{
    #if !defined(CONFIG_SOC_FAMILY_ESP32)
    memfault_arch_disable_configurable_faults();
    #endif
    int *bad = (int *) 0;
    *bad = -1;
}
K_TIMER_DEFINE(coredump_timer, coredump_timer_expired, NULL);

/* Golioth RPC for triggering an assert in order to capture a Memfault coredump */
static enum golioth_rpc_status coredump_rpc(zcbor_state_t *request_params_array,
                                        zcbor_state_t *response_detail_map,
                                        void *callback_arg)
{
    /* Assert after timer expiration to allow time for sending the RPC response */
    k_timer_start(&coredump_timer, K_SECONDS(5), K_SECONDS(0));
    LOG_INF("Coredump requested, resetting in 5 seconds");
    return GOLIOTH_OK;
}

/* Golioth RPC for triggering an immediate upload of any pending Memfault data */
static enum golioth_rpc_status memfault_upload_rpc(zcbor_state_t *request_params_array,
                                        zcbor_state_t *response_detail_map,
                                        void *callback_arg)
{
    /* This function is executed on the Golioth CoAP Client thread. To avoid
       deadlocking that thread, we will perform the blocking upload operation
       on the System Workqueue. */
    struct memfault_work_ctx *ctx = callback_arg;

    k_work_submit(&ctx->work);
    return GOLIOTH_OK;
}

/* Callback invoked by the Golioth SDK to read the next block of Memfault
   data to send to Goliot */
static enum golioth_status read_memfault_chunk(uint32_t block_idx,
                                               uint8_t *block_buffer,
                                               size_t *block_size,
                                               bool *is_last,
                                               void *arg)
{
    eMemfaultPacketizerStatus mflt_status;
    mflt_status = memfault_packetizer_get_next(block_buffer, block_size);
    if (kMemfaultPacketizerStatus_NoMoreData == mflt_status)
    {
        /* We should not hit this, but can recover */
        LOG_WRN("Unexpected end of Memfault data");
        *block_size = 0;
        *is_last = true;
    }
    else if (kMemfaultPacketizerStatus_EndOfChunk == mflt_status)
    {
        /* Last block */
        *is_last = true;
    }
    else if (kMemfaultPacketizerStatus_MoreDataForChunk == mflt_status)
    {
        *is_last = false;
    }

    return GOLIOTH_OK;
}

int golioth_memfault_upload(struct golioth_client *client)
{
    /* Configure the Memfault SDK to read maximum sized chunks across
       multiple calls */
    sPacketizerConfig cfg = {
        .enable_multi_packet_chunk = true,
    };
    sPacketizerMetadata metadata = {};
    bool data_available = memfault_packetizer_begin(&cfg, &metadata);
    while (data_available)
    {
        LOG_DBG("Uploading memfault data");
        /* TODO make this non-blocking */
        golioth_stream_set_blockwise_sync(client,
                                          "mflt",
                                          GOLIOTH_CONTENT_TYPE_OCTET_STREAM,
                                          read_memfault_chunk,
                                          NULL);

        /* Check for additional chunks */
        data_available = memfault_packetizer_begin(&cfg, &metadata);
    }

    return 0;
}

int golioth_memfault_init(struct golioth_client *client, struct golioth_rpc *rpc)
{
    /* Initialize periodic timer for uploading data to Memfault */
    k_work_init(&mflt_ctx.work, memfault_work_handler);
    mflt_ctx.client = client;
    k_timer_user_data_set(&mflt_upload_timer, &mflt_ctx);
    k_timer_start(&mflt_upload_timer, K_SECONDS(5), TIMER_PERIOD);

    /* Register RPCs. These aren't necessary for routing Memfault data through
       Golioth, but can be useful for testing and debugging */
    golioth_rpc_register(rpc, "coredump", coredump_rpc, NULL);
    golioth_rpc_register(rpc, "memfault_upload", memfault_upload_rpc, &mflt_ctx.work);

    return 0;
}
