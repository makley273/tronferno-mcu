/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#include "../userio/inout.h"

#if !defined MCU_ESP8266
#if __has_include("../sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif
#endif

#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION
#include "../sensitive/defaults.h"
#endif

#ifndef MY_FER_CENTRAL_UNIT_ID
#ifdef CONFIG_MY_FER_CENTRAL_UNIT_ID
#define MY_FER_CENTRAL_UNIT_ID CONFIG_MY_FER_CENTRAL_UNIT_ID
#else
#define MY_FER_CENTRAL_UNIT_ID 0x801234
#endif
#endif

#ifndef MY_MCU_UART_BAUD_RATE
#ifdef CONFIG_MY_MCU_UART_BAUD_RATE
#define MY_MCU_UART_BAUD_RATE CONFIG_MY_MCU_UART_BAUD_RATE
#else
#define MY_MCU_UART_BAUD_RATE 115200
#endif
#endif

// Berlin/Germany
#ifndef MY_GEO_LONGITUDE
#ifdef CONFIG_MY_GEO_LONGITUDE
#define MY_GEO_LONGITUDE CONFIG_MY_GEO_LONGITUDE
#else
#define MY_GEO_LONGITUDE +13.38
#endif
#endif

#ifndef MY_GEO_LATITUDE
#ifdef CONFIG_MY_GEO_LATITUDE
#define MY_GEO_LATITUDE CONFIG_MY_GEO_LATITUDE
#else
#define MY_GEO_LATITUDE +52.53
#endif
#endif

#ifndef MY_GEO_TZ
#ifdef CONFIG_MY_GEO_TZ
#define MY_GEO_TZ CONFIG_MY_GEO_TZ
#else
#define MY_GEO_TZ "CET-1CEST-2,M3.5.0,M10.5.0" // POSIX TZ format
#endif
#endif

#ifndef MY_GEO_TIMEZONE
#ifdef CONFIG_MY_GEO_TIMEZONE
#define MY_GEO_TIMEZONE CONFIG_MY_GEO_TIMEZONE
#else
#define MY_GEO_TIMEZONE +1.0
#endif
#endif

#ifndef MY_GEO_DST
#ifdef CONFIG_MY_GEO_DST
#define MY_GEO_DST CONFIG_MY_GEO_DST
#else
#define MY_GEO_DST dstEU
#endif
#endif

#ifndef MY_WIFI_SSID
#ifdef CONFIG_MY_WIFI_SSID
#define MY_WIFI_SSID CONFIG_MY_WIFI_SSID
#else
#define MY_WIFI_SSID ""
#endif
#endif

#ifndef MY_WIFI_PASSWORD
#ifdef CONFIG_MY_WIFI_PASSWORD
#define MY_WIFI_PASSWORD CONFIG_MY_WIFI_PASSWORD
#else
#define MY_WIFI_PASSWORD ""
#endif
#endif

#ifndef MY_NTP_SERVER
#ifdef CONFIG_MY_NTP_SERVER
#define MY_NTP_SERVER CONFIG_MY_NTP_SERVER
#else
#define MY_NTP_SERVER "pool.ntp.org"
#endif
#endif

#ifndef MY_APP_CONFIG_PASSWORD
#ifdef CONFIG_MY_APP_CONFIG_PASSWORD
#define MY_APP_CONFIG_PASSWORD CONFIG_MY_APP_CONFIG_PASSWORD
#else
#define MY_APP_CONFIG_PASSWORD ""
#endif
#endif

#ifndef MY_APP_EXPERT_PASSWORD
#ifdef CONFIG_MY_APP_EXPERT_PASSWORD
#define MY_APP_EXPERT_PASSWORD CONFIG_MY_APP_EXPERT_PASSWORD
#else
#define MY_APP_EXPERT_PASSWORD ""
#endif
#endif

#ifndef MY_MQTT_ENABLE
#ifdef CONFIG_MY_MQTT_ENABLE
#define MY_MQTT_ENABLE CONFIG_MY_MQTT_ENABLE
#else
#define MY_MQTT_ENABLE 0
#endif
#endif

#ifndef MY_MQTT_URL
#ifdef CONFIG_MY_MQTT_URL
#define MY_MQTT_URL CONFIG_MY_MQTT_URL
#else
#define MY_MQTT_URL ""
#endif
#endif

#ifndef MY_MQTT_USER
#ifdef CONFIG_MY_MQTT_USER
#define MY_MQTT_USER CONFIG_MY_MQTT_USER
#else
#define MY_MQTT_USER ""
#endif
#endif

#ifndef MY_MQTT_PASSWORD
#ifdef CONFIG_MY_MQTT_PASSWORD
#define MY_MQTT_PASSWORD CONFIG_MY_MQTT_PASSWORD
#else
#define MY_MQTT_PASSWORD ""
#endif
#endif

#ifndef MY_HTTP_ENABLE
#ifdef CONFIG_MY_HTTP_ENABLE
#define MY_HTTP_ENABLE CONFIG_MY_HTTP_ENABLE
#else
#define MY_HTTP_ENABLE 0
#endif
#endif

#ifndef MY_HTTP_USER
#ifdef CONFIG_MY_HTTP_USER
#define MY_HTTP_USER CONFIG_MY_HTTP_USER
#else
#define MY_HTTP_USER ""
#endif
#endif

#ifndef MY_HTTP_PASSWORD
#ifdef CONFIG_MY_HTTP_PASSWORD
#define MY_HTTP_PASSWORD CONFIG_MY_HTTP_PASSWORD
#else
#define MY_HTTP_PASSWORD ""
#endif
#endif

#ifndef MY_NETWORK_CONNECTION
#define MY_NETWORK_CONNECTION nwWlanAp
#define MY_NETWORK_CONNECTION_OLD_USERS nwWlanSta
#endif

/*
 esp-idf: kconfig does not support float numbers...
config MY_GEO_LONGITUDE
  float "geographical longitude"
  default +13.38

config MY_GEO_LATITUDE
  float "geographical latitude"
  default +52.53

 */

#ifndef MY_FER_GM_USE
#define MY_FER_GM_USE 0x77777777
#endif

#include "user_config.h"

enum dst {
	dstNone, dstEU, dstUS, dstAlways,
};

enum receiver {
	recvNone, recvTick
};

enum transmitter {
	transmNone, transmTick
};

enum rtclock {
	rtcNone, rtcAvrTime
};

enum verbosity {
	vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};

#ifdef USE_NETWORK
enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};
#endif

typedef struct {
	u32 fer_centralUnitID, mcu_serialBaud;
	float geo_longitude, geo_latitude;

	float geo_timezone;
#ifdef MDR_TIME
  enum dst geo_dST;
#endif
  i32 app_rtcAdjust;
	enum receiver app_recv;
	enum transmitter app_transm;
	enum rtclock app_rtc;
	enum verbosity app_verboseOutput;
	u32 fer_usedMembers; // each of the nibbles 1-7 stands for a group. nibble 1 == group 1. nibble 0 = number of used Groups (stored for the front-end, not used here on the MCU)

#ifdef USE_WLAN
	char wifi_SSID[32];
	char wifi_password[64];
#endif
	char app_configPassword[16];
	char app_expertPassword[16];
#ifdef ACCESS_GPIO
	 enum mcu_pin_state gpio[CONFIG_GPIO_SIZE];
#endif
#ifdef POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_MQTT
  char mqtt_url[64];
  char mqtt_user[16];
  char mqtt_password[31];
  i8 mqtt_enable;
#endif
#ifdef USE_HTTP
  char http_user[16];
  char http_password[31];
  i8 http_enable;
#endif
#ifdef USE_NTP
  char ntp_server[64];
#endif
#ifdef USE_NETWORK
  enum nwConnection network;
#endif
} config;

extern config C;

void read_config(u32 mask);  // restore C from persistent storage
void save_config(u32 mask);  // save C to persistent storage
enum {
CB_RECV,
CB_TRANSM,
CB_CUID,
CB_USED_MEMBERS,
CB_BAUD,
CB_GPIO,
CB_VERBOSE,
CB_WIFI_SSID,
CB_WIFI_PASSWD,
CB_CFG_PASSWD,
CB_LONGITUDE,
CB_LATITUDE,
CB_TIZO,
CB_TZ,
CB_DST,
CB_MQTT_URL,
CB_MQTT_USER,
CB_MQTT_PASSWD,
CB_MQTT_ENABLE,
CB_HTTP_USER,
CB_HTTP_PASSWD,
CB_HTTP_ENABLE,
CB_NTP_SERVER,
CB_NW_CONN,
};

#define CONFIG_ALL (~0UL)
#define CONFIG_RECV (1UL << CB_RECV)
#define CONFIG_TRANSM (1UL << CB_TRANSM)
#define CONFIG_CUID (1UL << CB_CUID)
#define CONFIG_USED_MEMBERS (1UL << CB_USED_MEMBERS)
#define CONFIG_BAUD (1UL << CB_BAUD)
#define CONFIG_GPIO (1UL << CB_GPIO)
#define CONFIG_VERBOSE (1UL << CB_VERBOSE)
#define CONFIG_WIFI_SSID (1UL << CB_WIFI_SSID)
#define CONFIG_WIFI_PASSWD (1UL << CB_WIFI_PASSWD)
#define CONFIG_CFG_PASSWD (1UL << CB_CFG_PASSWD)
#define CONFIG_LONGITUDE (1UL << CB_LONGITUDE)
#define CONFIG_LATITUDE (1UL << CB_LATITUDE)
#define CONFIG_TIZO (1UL << CB_TIZO)
#define CONFIG_TZ (1UL << CB_TZ)
#define CONFIG_DST (1UL << CB_DST)
#define CONFIG_MQTT_URL (1UL << CB_MQTT_URL)
#define CONFIG_MQTT_USER (1UL << CB_MQTT_USER)
#define CONFIG_MQTT_PASSWD (1UL << CB_MQTT_PASSWD)
#define CONFIG_MQTT_ENABLE (1UL << CB_MQTT_ENABLE)
#define CONFIG_HTTP_USER (1UL << CB_HTTP_USER)
#define CONFIG_HTTP_PASSWD (1UL << CB_HTTP_PASSWD)
#define CONFIG_HTTP_ENABLE (1UL << CB_HTTP_ENABLE)
#define CONFIG_NTP_SERVER (1UL << CB_NTP_SERVER)
#define CONFIG_NETWORK_CONNECTION (1UL << CB_NW_CONN)
#endif /* CONFIG_H_ */

