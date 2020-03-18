/*
 * commands.c
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#include "fernotron/fsb.h"
#include "fernotron/fer_msg_plain.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron_pos/commands.h"
#include "fernotron_pos/shutter_pct.h"
#include "app/proj_app_cfg.h"

#include "cli_app/cli_imp.h"
#include "cli_app/cli_fer.h"
#include "config/config.h"

#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7

bool   commands_sendShutterCommand(u32 a, u8 g, u8 m, fer_cmd cmd, u8 repeats) {
  fsbT *fsb = get_fsb(a,g,m,cmd);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
}

bool   commands_moveShutterToPct(u32 a, u8 g, u8 m, u8 pct, u8 repeats) {
  precond(g <= 7 && m <= 7 && pct <= 100);
  int curr_pct = -1;

  if (m == 0) {
    gm_bitmask_t gm = {0,};
    gm_SetByte(&gm, g, 0xfe);
    return commands_moveShutterMaskToPct(a, &gm, pct, repeats);
  }

  fer_cmd fc = fer_cmd_None;
  fer_grp group = g;
  fer_memb memb = m == 0 ? 0 : m + 7;

  if (g > 0 && m > 0) {
    curr_pct = ferPos_getPct(a, g, m);
  }

  fsbT *fsb = get_sender_by_addr(a);
  if (!fsb) {
    static fsbT fsb_direct; // FIXME: or was senders[0] meant for this?
    fsb = &fsb_direct;
    if (FSB_GET_DEVID(fsb) != a) {
      fer_init_sender(fsb, a);
    }
  }

  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    FSB_PUT_GRP(fsb, group);
    FSB_PUT_MEMB(fsb, memb);
  }

  if (pct == 0 || pct == 100) {
    fc = (pct == 0) ? fer_cmd_DOWN : fer_cmd_UP;
    FSB_PUT_CMD(fsb, fc);
    fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
  } else if (curr_pct >= 0) {
    u16 stop_delay = ferPos_calcMoveDuration_fromPctDiff_m(g, m, curr_pct, pct);
    if (stop_delay == 0)
      return false;
    fc = (pct < curr_pct) ? fer_cmd_DOWN : fer_cmd_UP;
    FSB_PUT_CMD(fsb, fc);
    fer_send_msg_with_stop(fsb, 0, stop_delay, repeats);
  } else {
    FSB_PUT_CMD(fsb, fer_cmd_UP);
    fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
    u16 delay = ferPos_calcMoveDuration_fromPctDiff_m(g, m, 0, 100);
    u16 stop_delay = ferPos_calcMoveDuration_fromPctDiff_m(g, m, 100, pct);
    if (stop_delay == 0)
      return false;
    FSB_PUT_CMD(fsb, fer_cmd_DOWN);
    fer_send_msg_with_stop(fsb, delay, stop_delay, repeats);
  }

  return true;
}

bool   commands_moveShutterMaskToPct(u32 a, gm_bitmask_t *gm, u8 pct, u8 repeats) {
  u8 g, m;

  for (g=1; g <= GRP_MAX; ++g) {
    u8 um = (C.fer_usedMembers >> (g*4)) & 0x0f;
    for (m=1; m <= MBR_MAX; ++m) {
      if (m > um)
        break;
      if (!gm_GetBit(gm, g, m))
        continue;

      if (!commands_moveShutterToPct(a, g, m, pct, 0)) {
        return false;
      }
    }
  }
  return true;
}
