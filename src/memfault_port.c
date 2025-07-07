/*
 * Copyright (c) 2025 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "memfault/components.h"

// Expand RAM range, e.g. to allow malloc-ed thread TCBs to be included in coredumps
size_t memfault_platform_sanitize_address_range(void *start_addr, size_t desired_size) {
  extern uint32_t __kernel_ram_start[];
  extern uint32_t __kernel_ram_end[];

  const uint32_t ram_start = (uint32_t)__kernel_ram_start;
  const uint32_t ram_end = (uint32_t)__kernel_ram_end;

  if ((uint32_t)start_addr >= ram_start && (uint32_t)start_addr < ram_end) {
    return MEMFAULT_MIN(desired_size, ram_end - (uint32_t)start_addr);
  }

  return 0;
}
