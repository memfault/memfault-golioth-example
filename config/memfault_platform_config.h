#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_SOC_FAMILY_ESP32)
  #define ZEPHYR_DATA_REGION_START __data_start
  #define ZEPHYR_DATA_REGION_END __data_end
#endif

// Force through the Devie Serial, overriding the uploading serial passed by
// the Golioth Pipeline
#define MEMFAULT_EVENT_INCLUDE_DEVICE_SERIAL 1

#ifdef __cplusplus
}
#endif
