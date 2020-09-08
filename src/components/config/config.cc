#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "config/config_defaults.h"
#include "cli_app/fernotron.h"
#include "misc/int_types.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define CI(cb) static_cast<configItem>(cb)

config C;

#ifdef POSIX_TIME
double tz2offset(const char *tz) {
  double offset = 0.0;

  for (const char *p = tz; *p; ++p) {
    if (isalpha(*p))
      continue;
    offset = strtod(p, 0) * -1;
    return offset;
  }
  return 0;
}

#endif
#ifdef POSIX_TIME
const char* config_read_tz(char *d, unsigned d_size) {
  return config_read_item_s(CI(CB_TZ), d, d_size, MY_GEO_TZ);
}
#endif
#ifdef MDR_TIME
float config_read_timezone() {
  return config_read_item_f(CI(CB_TIZO), MY_GEO_TIMEZONE);
}

enum dst config_read_dst() {
  return config_read_item_i8(CI(CB_DST), MY_GEO_DST);
}
#endif
#ifdef USE_NETWORK
enum nwConnection config_read_network_connection() {
  return static_cast<enum nwConnection>(config_read_item_i8(CI(CB_NETWORK_CONNECTION), MY_NETWORK_CONNECTION));
}
#endif

#ifndef MCU_ESP32
uint32_t config_read_baud() {
  return config_read_item_u32(CI(CB_BAUD), MY_MCU_UART_BAUD_RATE);
}
#endif

