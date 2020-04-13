/*
 * pin.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "../gpio/pin.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"

#include "txtio/inout.h"

#include "app/common.h"
#include "config/config.h"

#define printf ets_uart_printf


#ifndef USE_LAN
#define RFOUT_GPIO GPIO_NUM_22
#define RFIN_GPIO GPIO_NUM_17
#else
#define RFOUT_GPIO GPIO_NUM_16
#define RFIN_GPIO GPIO_NUM_15 //XXX: used by JTAG?
#endif

#define BUTTON_GPIO GPIO_NUM_6

#define RESERVED_GPIO (1ULL<<RFOUT_GPIO|1ULL<<RFIN_GPIO|1ULL<<BUTTON_GPIO)

#define gpioUsable  (0xffffffff & ~(1ULL<<0|1ULL<<1|1ULL<<2|1ULL<<20|1ULL<<24|1ULL<<28|1ULL<<29|1ULL<<30|1ULL<<31|RESERVED_GPIO)) \
  & (0xffffffff | (1ULL<<(GPIO_NUM_32-32)|1ULL<<(GPIO_NUM_33-32)))
const uint64_t inputGpioUsable = gpioUsable;
const uint64_t outputGpioUsable = gpioUsable;

#define gpioUsableHigh


const char*  mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state);

void gpio_get_levels(uint64_t gpio_mask, char *buf, int buf_size) {
  int i;
  int buf_len = (buf_size > 40 ? 39 : buf_size - 1);
  buf[buf_len] = '\0';

  for (i=0; i < buf_len; ++i) {
    if (1ULL<<i & gpioUsable) {
      buf[i] = gpio_get_level(i) ? '1' : '0';
    } else {
      buf[i] = 'x';
    }
  }

}

bool is_gpio_number_usable(int gpio_number, bool cli) {
  return  (gpioUsable & 1ULL<<gpio_number) != 0;
}

void IRAM_ATTR mcu_put_txPin(u8 dat) {
  GPIO_OUTPUT_SET(RFOUT_GPIO, dat);
}

u8 IRAM_ATTR mcu_get_rxPin() {
    return GPIO_INPUT_GET(RFIN_GPIO);
}

void
setup_pin(void) {
  int i;

  for (i = 0; i < CONFIG_GPIO_SIZE; ++i) {
    mcu_pin_state state = C.gpio[i];
    if (state == PIN_DEFAULT)
      continue;
    else if (state == PIN_INPUT || state == PIN_INPUT_PULLUP || state == PIN_OUTPUT)
      mcu_access_pin2(i, NULL, state);
    else if (state == PIN_SET || state == PIN_CLEAR) {
      mcu_access_pin2(i, NULL, PIN_OUTPUT);
      mcu_access_pin2(i, NULL, state);
    }
  }

  mcu_access_pin2(RFOUT_GPIO, NULL, PIN_OUTPUT);
  mcu_access_pin2(RFIN_GPIO, NULL, PIN_INPUT);
  mcu_access_pin2(BUTTON_GPIO, NULL, PIN_INPUT_PULLUP);

  mcu_put_txPin(false); // make sure, the RF transmitter is off
}

bool mcu_get_buttonUpPin(void) {
  return false;
}

bool mcu_get_buttonDownPin(void) {
  return false;
}

bool mcu_get_buttonPin(void) {
  bool val = gpio_get_level(BUTTON_GPIO);
  return val == 0;  // 0 means pressed
}


const char* mcu_access_pin2(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {

  if (state == PIN_OUTPUT || state == PIN_INPUT || state == PIN_INPUT_PULLUP) {
    //bool pullup = state == PIN_INPUT_PULLUP;

    if (state != PIN_OUTPUT) {
      GPIO_DIS_OUTPUT(GPIO_ID_PIN(gpio_number));
    }

    switch (gpio_number) {
    case GPIO_NUM_3:
    case GPIO_NUM_4:
    case GPIO_NUM_5:
    case GPIO_NUM_6:
    case GPIO_NUM_7:
    case GPIO_NUM_8:
    case GPIO_NUM_9:
    case GPIO_NUM_10:
    case GPIO_NUM_11:
    case GPIO_NUM_12:
    case GPIO_NUM_13:
    case GPIO_NUM_14:
    case GPIO_NUM_15:
    case GPIO_NUM_16:
    case GPIO_NUM_17:
    case GPIO_NUM_18:
    case GPIO_NUM_19:

    case GPIO_NUM_21:
    case GPIO_NUM_22:
    case GPIO_NUM_23:

    case GPIO_NUM_25:
    case GPIO_NUM_26:
    case GPIO_NUM_27:

    case GPIO_NUM_32:
    case GPIO_NUM_33:
      if (state == PIN_OUTPUT) {
        gpio_set_direction(gpio_number, GPIO_MODE_OUTPUT);
    } else if (state == PIN_INPUT) {
        gpio_set_direction(gpio_number, GPIO_MODE_INPUT);
    }

      break;

    case GPIO_NUM_34:
    case GPIO_NUM_35:
    case GPIO_NUM_36:
    case GPIO_NUM_37:
    case GPIO_NUM_38:
    case GPIO_NUM_39:
        if (state == PIN_OUTPUT) {
          return "GPIOs 34..39 can only used for input";
      } else if (state == PIN_INPUT) {
          gpio_set_direction(gpio_number, GPIO_MODE_INPUT);
      }

      break;

     default:
      return "gpio number can not be used";
    }

  } else if (state == PIN_SET) {
    gpio_set_level(gpio_number, 1);

  } else if (state == PIN_CLEAR) {
    gpio_set_level(gpio_number, 0);

  } else if (state == PIN_TOGGLE) {
    gpio_set_level(gpio_number, !gpio_get_level(gpio_number)); // TODO: works only for GPIO_MODE_INPUT_OUTPUTINPUT_OUTPUT

  } else if (state == PIN_READ) {
    *result = gpio_get_level(GPIO_ID_PIN(gpio_number)) ? PIN_SET : PIN_CLEAR;

  } else {
    return "not implemented";
  }

  return NULL;

}

const char *mcu_access_pin(int gpio_number, mcu_pin_state *result, mcu_pin_state state) {
#ifdef DISTRIBUTION
  if (state != PIN_READ)
    switch (gpio_number) {
   case 12:
   case 13:
   case 14:
   case 15:
     break;

   default:
     return "gpio number not writable (use 12, 13, 14, 15)";  // FIXME:
   }
#endif

    return mcu_access_pin2(gpio_number, result, state);
}
