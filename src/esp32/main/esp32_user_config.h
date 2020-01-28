#ifdef ESP32_USER_CONFIG_H_
#error should only inlcuded by root user_config.h
#endif
#define ESP32_USER_CONFIG_H_

#define MCU_TYPE "esp32"

//extern const char *const build_date;
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//#undef FER_TRANSMITTER
//#undef FER_RECEIVER
//#undef USE_NTP
//#undef USE_WLAN
//#undef USE_MQTT

#undef CONFIG_BLOB
//#undef ACCESS_GPIO
#define CONFIG_GPIO_SIZE 40
//#undef POSIX_TIME
#undef MDR_TIME
//#undef USE_PAIRINGS

#ifdef BOARD_ESP32GATEWAY
#define OLIMEX_ESP32_GATEWAY
#endif
#ifdef BOARD_ESP32POE
#define OLIMEX_ESP32_POE
#endif


#define SAVE_TIMER_DATA_FUN save_timer_data_fs
#define READ_TIMER_DATA_FUN read_timer_data_fs

