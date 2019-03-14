/*
 * timer_data_spiffs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_TIMER_DATA_FS_H_
#define USER_MAIN_TIMER_DATA_FS_H_

#include <stdint.h>
#include "timer_data.h"


bool save_timer_data_fs(timer_data_t *p, uint8_t group, uint8_t member);
bool read_timer_data_fs(timer_data_t *p, uint8_t *group, uint8_t *member, bool wildcard);

#endif /* USER_MAIN_TIMER_DATA_FS_H_ */
