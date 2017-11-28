/* -*- compile-command: "/cygdrive/e/WinAVR-20100110/utils/bin/make.exe -C default  all && bash -c ./__flash.sh"; -*- */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "rtc.h"

#include "all.h"
#include "set_endpos.h"

extern fer_sender_basic default_sender;
extern fer_sender_basic last_received_sender;

bool ICACHE_FLASH_ATTR cu_auto_set(unsigned init_seconds);


#define ENABLE_SET_ENDPOS 1

#if ENABLE_SET_ENDPOS

#endif




void ICACHE_FLASH_ATTR
loop(void) {
	char *cmdline;
	static bool ready;

	fers_loop();

#if ENABLE_SET_ENDPOS
	sep_loop();
#endif

	// CLI
	if ((cmdline = get_commandline())) {
		io_putlf();
		process_cmdline(cmdline);
		ready = false;
	} else if (!ready) {
		cli_msg_ready();
		ready = true;
	}

#ifdef FER_RECEIVER
	if (MessageReceived != MSG_TYPE_NONE) {
		switch (MessageReceived) {
		case MSG_TYPE_PLAIN:
			memcpy(&last_received_sender.data, rxmsg->cmd, 5);
			cu_auto_set(0);
			io_puts("R:"), fmsg_print(rxmsg, MessageReceived);
			break;
#ifndef	FER_RECEIVER_MINIMAL
		case MSG_TYPE_RTC:
		case MSG_TYPE_TIMER:
			io_puts("timer frame received\n"), fmsg_print(rxmsg, MessageReceived);
			break;
#endif
		}

		if (!fmsg_verify_checksums(rxmsg, MessageReceived)) {
			io_puts("checksum(s) wrong\n");
		}

		frx_clear();
	}
#endif

#ifdef USE_NTP
	ntp_update_system_time(SECS_PER_DAY);
#endif
}

void ICACHE_FLASH_ATTR
print_startup_info(void) {
#define slf ";\n"
#define cfg "config "

	if (C.app_verboseOutput >= vrbNone) {
		io_puts("\n\n" "Tronferno-MCU starting ...\n" "build-date: " __DATE__ " " __TIME__ "\n\n");
		io_puts(cfg "cu="), io_print_hex_32(C.fer_centralUnitID, false), io_puts(" baud="), io_print_dec_32(C.mcu_serialBaud, false), io_puts(slf);
		io_puts(cfg "longitude="), io_print_float(C.geo_longitude, 5), io_puts(" latitude="), io_print_float(C.geo_latitude, 5), io_puts(" time-zone="), io_print_float(C.geo_timezone, 2), io_puts(slf);
#ifdef USE_WLAN
		io_puts(cfg "wlan-ssid=\""), io_puts(C.wifi_SSID), io_puts("\" wlan-password="), io_puts((C.wifi_password[0] == '\0') ? "\"\""slf : "\"***********\"" slf);
#endif
		io_puts("\n(hint: type help; to get a command list)\n");
	}
}


int ICACHE_FLASH_ATTR
main_setup() {

	rtc_setup();
	fer_init_sender(&default_sender, C.fer_centralUnitID);
#ifdef DEBUG
	test_modules();
#endif

	print_startup_info();

	db_test_all_indicators(3);

	dbg_trace();
	return 0;

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test
