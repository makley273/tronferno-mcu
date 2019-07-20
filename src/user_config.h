// User configuration
#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_



#define APP_VERSION "0.8.0-beta"


#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NETWORK
#define USE_NTP
#define USE_WLAN
#define USE_WLAN_AP
#define USE_LAN
#define USE_MQTT
#define USE_HTTP
#define CONFIG_BLOB
#define CONFIG_DICT
#define ACCESS_GPIO
#define POSIX_TIME 1
#define MDR_TIME
#define USE_PAIRINGS
#define USE_JSON
#define USE_MUTEX

#define OLIMEX_ESP32_GATEWAY // XXX

// ESP8266 configuration
#if defined MCU_ESP8266
#include "esp8266/esp8266_user_config.h"
#elif defined MCU_ESP32
#include "esp32/main/esp32_user_config.h"
#endif

#define ENABLE_SET_ENDPOS 1

#include "main/common.h"

#endif /* USER_CONFIG_H_ */
