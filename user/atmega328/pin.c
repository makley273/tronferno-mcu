/*
 * pin.c
 *
 * Created: 12.09.2017 01:58:00
 *  Author: bertw
 */ 
#include "main/common.h"
#include "defines.h"

#define FER_MCUPIN_RECV  (D, 2) // Nano D2
#define FER_MCUPIN_SEND  (B, 3) // Nano D11


bool  mcu_get_rxPin() { return GETPIN(FER_MCUPIN_RECV); }


void mcu_put_txPin(bool dat) { PUTPIN(FER_MCUPIN_SEND, (dat)); }
  
  void setup_pin(void)
  {
    
    SET_DDR_IN(FER_MCUPIN_RECV);
    SET_DDR_OUT(FER_MCUPIN_SEND);
    mcu_put_txPin(0); // make sure the transmitter is off
  }
