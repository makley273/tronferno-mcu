/*
* astro.c
*
* Created: 24.08.2017 22:02:54
*  Author: bertw
*/

#include <stdlib.h>
#include <math.h>
#include "time.h"

#include "astro.h"
#include "common.h"
#include "utils.h"
#include "rtc.h"
#include "config.h"
#include "fer_code.h"
#include "fer.h"



#define CIVIL_TWILIGHT_RAD         -0.10471975511966
#define NAUTICAL_TWILIGHT_RAD      -0.20943951023932
#define ASTRONOMICAL_TWILIGHT_RAD  -0.31415926535898


static void ICACHE_FLASH_ATTR
calc_sunrise_sunset(float *sunrise, float *sunset, float timezone, int day_of_year, float dgrad_longitude, float dgrad_latidude)
{
  int T = day_of_year;
  float B = M_PI * dgrad_latidude / 180;
  float h = CIVIL_TWILIGHT_RAD;
  float Deklination = 0.4095*sin(0.016906*(T-80.086));
  float Zeitdifferenz = 12*acos((sin(h) - sin(B)*sin(Deklination)) / (cos(B)*cos(Deklination)))/M_PI;
  float Zeitgleichung = -0.171*sin(0.0337*T + 0.465) - 0.1299*sin(0.01787*T - 0.168);

  if (sunrise) {
    float sunrise_woz = (12 - Zeitdifferenz - Zeitgleichung);
    float sunrise_gmt = sunrise_woz + dgrad_longitude/15;
    *sunrise = sunrise_gmt + timezone;
  }
  
  if (sunset) {
    float sunset_woz = (12 + Zeitdifferenz - Zeitgleichung);
    float sunset_gmt = sunset_woz + dgrad_longitude/15;
    *sunset = sunset_gmt + timezone;
  }
}

static void ICACHE_FLASH_ATTR
time_to_bcd(uint8_t *dstMinutes, uint8_t *dstHours, float time, bool force_even_minutes)
{
  double integral, fractional;
  
  fractional = modf(time, &integral);
  
  if (dstHours) {
    *dstHours = dec2bcd_special((uint8_t)integral);
  }
  if (dstMinutes) {
    *dstMinutes = dec2bcd_special((force_even_minutes ? ~1 : ~0) & (uint8_t)(fractional * 60));
  }
}


static void ICACHE_FLASH_ATTR
math2_write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{
	int i, j;
  time_t day_stamp = 355 * ONE_DAY;
 
    for (i=0; i < 12; ++i) {
      for (j=0; j < 8; ++j, (day_stamp += 329400)) {
        time_t sunset = sun_set(&day_stamp) + (mint_offset * SECS_PER_MINT);
        struct tm *t = gmtime(&sunset);
        dst[i][j] = dec2bcd_special(t->tm_min);
        dst[i][++j] = dec2bcd_special(t->tm_hour);
      }
    }
}  


static void ICACHE_FLASH_ATTR
math_write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
  float sunset = 0, day = 355;
  
    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j, (day += 3.4)) {
        calc_sunrise_sunset(NULL, &sunset, C.geo_timezone + (mint_offset / 60.0f), ((int)day) % 366, C.geo_longitude, C.geo_latitude);
        time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset, true);
      }
    }


}  

// day table generated by comparing original astro table and sunset formula
// FIXME: day table should be replaced by formula
const int16_t day_table[12][4] = {
#if 1
		{ 355, 357, 357, 361,  },
		{ 364, 1, 4, 7,  },
		{ 12, 16, 20, 24,  },
		{ 29, 33, 37, 42,  },
		{ 46, 50, 55, 60,  },
		{ 65, 69, 74, 78,  },
		{ 83, 87, 93, 97,  },
		{ 101, 106, 111, 115,  },
		{ 120, 124, 128, 132,  },
		{ 136, 140, 144, 148,  },
		{ 152, 155, 159, 163,  },
		{ 165, 165, 165, 165,  },
#else
		{ 355, 359, 359, 362,  },
		{ 365, 1, 4, 7,  },
		{ 11, 15, 19, 22,  },
		{ 27, 30, 34, 39,  },
		{ 42, 46, 51, 56,  },
		{ 60, 65, 69, 74,  },
		{ 78, 83, 88, 93,  },
		{ 97, 103, 108, 112,  },
		{ 118, 123, 128, 132,  },
		{ 138, 142, 148, 153,  },
		{ 159, 164, 173, 177,  },
		{ 177, 177, 177, 177,  },
#endif
};


static void ICACHE_FLASH_ATTR
math_tbl_write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
  float sunset = 0, day = 355;

    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j) {
        calc_sunrise_sunset(NULL, &sunset, C.geo_timezone + (mint_offset / 60.0f), (int)day_table[i][j], C.geo_longitude, C.geo_latitude);
        time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset, true);
      }
    }

}

static void ICACHE_FLASH_ATTR
math_write_astro2(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
  float sunset = 0, day = 355;

    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j, (day += 4)) {
        calc_sunrise_sunset(NULL, &sunset, C.geo_timezone + (mint_offset / 60.0f), ((int)day) % 366, C.geo_longitude, C.geo_latitude);
        time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset, true);
      }
    }
}

static void ICACHE_FLASH_ATTR
dbg_write_astro(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;

    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j) {
    	  dst[i][j*2] = 0x18;
	      dst[i][j*2+1] = 0x18;
      }
    }
}

static void ICACHE_FLASH_ATTR
dbg_write_astro2(uint8_t dst[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset)
{ int i, j;
 float sunset = 18;

    for (i=0; i < 12; ++i) {
      for (j=0; j < 4; ++j) {
          time_to_bcd(&dst[i][j*2],&dst[i][j*2+1], sunset, true);

          sunset += 1.0/60;
      }
    }
}

#if TEST_MODULE_ASTRO
uint8_t data[12][8];

bool ICACHE_FLASH_ATTR
testModule_astro()
{
  float rise, set;
  uint16_t doy = 172;
  calc_sunrise_sunset(&rise, &set, C.timezone, doy, C.geo_longitude, C.geo_latitude);
  
  fill_astro(data, C.geo_timezone, 0, C.geo_longitude, C.geo_latitude);
  

  return rise == set;
}

#endif


static void
ICACHE_FLASH_ATTR tbl_write_astro(uint8_t d[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], const uint8_t ad[12][8], int mint_offset) {
  int col, line;

  mint_offset &= ~1;  // make sure the number is even


  for (line=0; line < FPR_ASTRO_HEIGHT; ++line) {
    for (col=0; col < FPR_ASTRO_WIDTH; ++col) {
      d[line][col] = dec2bcd(bcd2dec(ad[line][col]) + (mint_offset % 60));
      ++col;
      d[line][col] = dec2bcd(bcd2dec(ad[line][col]) + (mint_offset / 60));
    }
  }
}


// real times: 2017-10-22T18:27:00 (DST)  // the device interpolates times from the table

// berlin, germany
const uint8_t ad_plz_10[12][8]  = {
//                       01.01  //FIXME: the dates in comments are mostly wrong, because the debug data with 1 minute difference table confuses the receiver
{0x34, 0x16, 0x36, 0x16, 0x36, 0x16, 0x38, 0x16, }, // jan1, dec2
//            01.12.
{0x40, 0x16, 0x44, 0x16, 0x48, 0x16, 0x52, 0x16, }, // jan2, dec1
//
{0x58, 0x16, 0x04, 0x17, 0x10, 0x17, 0x16, 0x17, }, // feb1, nov2
//            01.11        (27.10!)
{0x24, 0x17, 0x30, 0x17, 0x38, 0x17, 0x46, 0x17, }, // feb2, nov1
//  01.02    01.03
{0x52, 0x17, 0x00, 0x18, 0x08, 0x18, 0x18, 0x18, }, // mar1, oct2
//
{0x26, 0x18, 0x34, 0x18, 0x42, 0x18, 0x50, 0x18, }, // mar2, oct1
// 01.10 ??? 01.04
{0x58, 0x18, 0x06, 0x1f, 0x16, 0x1f, 0x24, 0x1f, }, // apr1, sept2
// 01.09
{0x32, 0x1f, 0x42, 0x1f, 0x50, 0x1f, 0x58, 0x1f, }, // apr2, sept1
// 01.05
{0x08, 0x20, 0x16, 0x20, 0x24, 0x20, 0x32, 0x20, }, // may1, aug2
// 01.08
{0x40, 0x20, 0x46, 0x20, 0x54, 0x20, 0x00, 0x21, }, // may2, aug1
//
{0x06, 0x21, 0x10, 0x21, 0x14, 0x21, 0x18, 0x21, }, // jun1, jul2
// 01.07                  01.06
{0x20, 0x21, 0x20, 0x21, 0x20, 0x21, 0x20, 0x21, }, // jun2, jul1
};



// berlin actual sunset:    15:54 ... 20:36 (GMT + 1)
// berlin fernotron table:  16:34 ... 21:20  // shutters close one hour after sunset by default?  Has nothing to do with DST

void ICACHE_FLASH_ATTR write_astro(uint8_t d[FPR_ASTRO_HEIGHT][FER_PRG_BYTE_CT], int mint_offset) {
#if 0
  tbl_write_astro(d, ad_plz_10, mint_offset);
#elif 1
  math_tbl_write_astro(d, mint_offset);
#elif 0
  math_write_astro(d, mint_offset + 60); // FIXME: added one hour for correction to be consistent with astro original table. (see above)
#elif 1
  math2_write_astro(d, mint_offset);
#else
  math_write_astro2(d, mint_offset);
#endif
}
