#pragma once
#include <app/config/proj_app_cfg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <debug/dbg.h>


struct Fer_TimerData {
  /*
   * daily=T - sets the daily timer
   * T  - T is a 8 digit time string like 07302000. The four left digits are the up-time. The four on the right the down-time. A minus sign can replace 4 digits, which means the timer is cleared.
   *
   * timer daily=07302000;   up 07:30, down 20:00
   * timer daily=0730-;      up 07:30, not down
   * timer daily=-2000;      not up,   down 20:00
   */
  bool hasDaily() const {
    return daily[0] != '\0';
  }
  const char* getDaily() const {
    return daily;
  }
  void putDaily(const char *dt) {
    precond(!dt || strlen(dt) < sizeof daily);
    if (!dt)
      daily[0] = '\0';
    else
      STRLCPY(daily, dt, sizeof daily);
  }

  /*
   * weekly=TTTTTTT - sets a timer for each week day. week days are from left to right: Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
   * T - Each T is a 8 digit time string like described above with daily option.  A plus sign repeats the previous T.  So you can copy the values from Monday to Tuesday and so on.
   *
   * timer weekly=0730-++++0900-+;    up Monday-Friday at 07:30, and Saturday-Sunday at 09:00
   *
   *
   */
  bool hasWeekly() const {
    return weekly[0] != '\0';
  }
  const char* getWeekly() const {
    return weekly;
  }
  void putWeekly(const char *wt) {
    precond(!wt || strlen(wt) < sizeof weekly);
    if (!wt)
      weekly[0] = '\0';
    else
      STRLCPY(weekly, wt, sizeof weekly);
  }

  /*
   *
   *  astro=-30;    closes shutter thirty minutes before civil dusk`
   *  astro=+120;   closes shutter 2 hours after civil dusk`
   */
  bool hasAstro() const {
    return astro < 20000;
  }
  int getAstro() const {
    return astro;
  }
  void putAstro(int minOffset) {
    astro = minOffset;
  }
  void putAstro(bool enable) {
    enable ? astro = 0 : astro = 20000;
  }

  /*
   * enable/disable built-in random timer
   */
  bool getRandom() const {
    return flags.random;
  }
  void putRandom(bool val) {
    flags.random = val;
  }

  /*
   * enable/disable sun shade automatic
   */
  void putSunAuto(bool val) {
    flags.sunAuto = val;
  }
  bool getSunAuto() const {
    return flags.sunAuto;
  }

private:
  int16_t astro = 20000;      // minute offset of civil dusk, or 20000 for disables astro
  union {
    struct {
      uint8_t random :1;
      uint8_t unused_1 :1;
      uint8_t dst :1;
      uint8_t unused_3 :1;
      uint8_t unused_4 :1;
      uint8_t unused_5 :1;
      uint8_t unused_6 :1;
      uint8_t sunAuto :1;
    } flags;
    uint8_t bf = 0;         // bitfield
  };
  char daily[8 + 1] = "";    // ASCII string of one daily timer
  char weekly[7 * 8 + 1] = ""; // ASCII string of seven weekly timers
};
