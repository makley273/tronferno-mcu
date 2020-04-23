#include "main.h"

#include "net/http/server/http_server.h"
#include "net/tcp_cli_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <lwip/apps/sntp.h>
#include <lwip/apps/sntp_opts.h>
#include <esp_attr.h>


SemaphoreHandle_t uart_mutex;
i32 boot_counter;

void lfa_createWifiAp() {
  static bool wifi_ap_active;

  if (!wifi_ap_active) {
    wifi_ap_active = true;
    wifiAp_setup(WIFI_AP_SSID, WIFI_AP_PASSWD);

    struct cfg_tcps cfg_tcps = { .enable = true };
    tcpCli_setup(&cfg_tcps);

    struct cfg_http cfg_http = { .enable = true };
    hts_setup(&cfg_http);
  }
}

void lfa_gotIpAddr(void) {
  ipnet_connected();
}
void lfa_lostIpAddr(void) {
  ipnet_disconnected();
}

void appEsp32_main(void) {

  mcu_init();

  tmr_loopPeriodic_start();
  while (1) {
    loop();
#ifndef USE_EG
    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
#endif
  }
}

void  mcu_restart(void) {
  printf("mcu_restart()\n");
  ets_delay_us(10000);
  esp_restart();
  for (;;) {
  };
}

#ifdef USE_EG
void IRAM_ATTR lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

  xResult = xEventGroupSetBitsFromISR(
      loop_event_group,  // The event group being updated.
      uxBitsToSet, // The bits being set.
      &xHigherPriorityTaskWoken );

  // Was the message posted successfully?
  if (yield && xResult == pdPASS) {
    portYIELD_FROM_ISR();
  }
}
#endif
