/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "timer_data.h"
#include "timer_data_fs.h"
#include "misc/int_types.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
bool timer_data_changed;


bool  save_timer_data(timer_data_t *p, u8 g, u8 m) {
  bool result = save_timer_data_fs(p, g, m);
#ifdef LEGACY_STORAGE_2_WRITE
  if (!result)
    result = save_timer_data_old(p, g, m);
#endif
  timer_data_changed = true;
  fau_TIMER_DATA_CHANGE_cb();
  return result;
}

bool  read_timer_data(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  bool result = read_timer_data_fs(p, g, m, wildcard);
#ifdef LEGACY_STORAGE_2_READ
  if (!result)
    result = read_timer_data_old(p, g, m, wildcard);
#endif
  return result;
}
