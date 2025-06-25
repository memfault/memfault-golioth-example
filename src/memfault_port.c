/*
 * Copyright (c) 2025 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "memfault/components.h"

void memfault_platform_get_device_info(sMemfaultDeviceInfo *info) {
  // *NOTE* All strings must be populated, and the values assigned to 
  // the fields must have static lifetime: the data is accessed when 
  // this function returns.
  
  *info = (sMemfaultDeviceInfo) {
    // Device serial will be reported by Golioth, therefore set this to a
    // dummy value
    .device_serial = "NULL",
    // Set the device software type- it can be simply "app" for a single-
    // chip device, otherwise it should match the component name, eg "ble"
    // "sensor" etc.
    // This is used to filter data in the Memfault UI
    .software_type = "app",
    // Set the device software version.
    // This is used to filter devices in the Memfault UI. If using Memfault
    // OTA, this should exactly match the OTA Release Version name for the
    // installed image.
    .software_version = "0.0.1-dev",
    // Set the device hardware revision.
    // This is used to filter/group devices in the Memfault UI
    .hardware_version = "evt",
  };
}
