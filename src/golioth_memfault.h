/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <golioth/client.h>
#include <golioth/rpc.h>

int golioth_memfault_upload(struct golioth_client *client);
int golioth_memfault_init(struct golioth_client *client, struct golioth_rpc *rpc);
