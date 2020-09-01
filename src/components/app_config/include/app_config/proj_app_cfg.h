// User configuration
#pragma once

#define APP_VERSION "0.10.9"

#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NETWORK
#define USE_NTP
#define USE_WLAN
#define USE_WLAN_AP
#define USE_LAN
#define USE_TCPS
#define USE_TCPS_TASK
#define USE_MQTT
#define USE_WS
#define USE_HTTP
#define CONFIG_BLOB
#define CONFIG_DICT
#define USE_CONFIG_KVS
#define ACCESS_GPIO
#define POSIX_TIME 1
#define USE_ESP_GET_TIME
#define MDR_TIME
#define USE_PAIRINGS
#define USE_CUAS
#define USE_JSON
#define USE_CLI_MUTEX
#define USE_CLI_TASK
#define USE_OTA
#define USE_TXTIO_MUTEX

//#define USE_PCT_ARRAY

#define USE_FREERTOS

#include "callbacks.h"

#include "misc/int_types.h"

#define USE_SEP
#define ENABLE_SET_ENDPOS 1

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif

#define USE_EG
#define USE_AP_FALLBACK
#define CHECK_NETWORK_INTERVAL 15
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

#define WIFI_AP_SSID "tronferno"
#define WIFI_AP_PASSWD "tronferno"

#define USE_TIMER_DATA_FS
#define USE_TIMER_DATA_KVS
#define TD_FS_TO_KVS

#if defined MCU_ESP8266
#include "app_config/esp8266/esp8266_user_config.h"
#elif defined MCU_ESP32
#include "app_config/esp32/esp32_user_config.h"
#elif defined TEST_HOST
#define IRAM_ATTR
#undef ACCESS_GPIO
#undef USE_ESP_GET_TIME
#undef MDR_TIME
#define MY_RFOUT_GPIO -1
#define MY_RFIN_GPIO -1
#define MY_SETBUTTON_GPIO -1
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)
#define MCU_TYPE "host"
#else
#define IRAM_ATTR
#define DRAM_ATTR
#error "no supported MCU"
#endif


