/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <golioth/client.h>
#include <golioth/rpc.h>

#include <memfault/components.h>

#include <samples/common/sample_credentials.h>
#include <samples/common/net_connect.h>

#include "golioth_memfault.h"

static K_SEM_DEFINE(connected, 0, 1);
#define BUILD_ID_LEN 6

static void on_golioth_client_event(struct golioth_client *client, enum golioth_client_event event, void *arg) {
    bool is_connected = (event == GOLIOTH_CLIENT_EVENT_CONNECTED);
    if (is_connected) {
        k_sem_give(&connected);
    }
    LOG_INF("Golioth client %s", is_connected ? "connected" : "disconnected");
}

int main(void)
{
    LOG_INF("Starting Golioth + Memfault example application");

    net_connect();

    const struct golioth_client_config *client_config = golioth_sample_credentials_get();

    struct golioth_client *client = golioth_client_create(client_config);
    struct golioth_rpc *rpc = golioth_rpc_init(client);

    golioth_client_register_event_callback(client, on_golioth_client_event, NULL);

    k_sem_take(&connected, K_FOREVER);

    golioth_memfault_init(client, rpc);

    while(true)
    {
        k_sleep(K_FOREVER);
    }
}
