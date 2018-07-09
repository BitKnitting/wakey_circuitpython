/*
  RTC library for Arduino Zero.
  Copyright (c) 2015 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "Extint_lib.h"
#include <wiring_private.h>

static voidFuncPtr ISRcallback[EXTERNAL_NUM_INTERRUPTS];
static uint32_t    ISRlist[EXTERNAL_NUM_INTERRUPTS];
static uint32_t    nints; // Stores total number of attached interrupts
static uint32_t    inMask;


Extint::Extint()
{
  _configured = false;
}

void Extint::begin()
{

  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);
  NVIC_SetPriority(EIC_IRQn, 0);
  NVIC_EnableIRQ(EIC_IRQn);
  // Enable EIC
  EIC->CTRL.bit.ENABLE = 1;
  while (EIC->STATUS.bit.SYNCBUSY != 0) {}
  _configured = true;

}
bool Extint::attachI(uint32_t pin, voidFuncPtr callback, uint32_t mode)
{
  static int enabled = 0;
  uint32_t config;
  uint32_t pos;

#if ARDUINO_SAMD_VARIANT_COMPLIANCE >= 10606  // Which we are....
// see https://github.com/arduino/ArduinoCore-samd/blob/master/variants/arduino_zero/variant.cpp
  EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
#else
// We don't use this...
  EExt_Interrupts in = digitalPinToInterrupt(pin);
#endif
  if (in == NOT_AN_INTERRUPT || in == EXTERNAL_INT_NMI)
    return false;

if (!_configured) {
  begin();
  _configured = 1;
}

// Enable wakeup capability on pin in case being used during sleep
inMask = 1 << in;
EIC->WAKEUP.reg |= inMask;

// Assign pin to EIC...Arduino function...leave for now...
pinPeripheral(pin, PIO_EXTINT);

// Only store when there is really an ISR to call.
// This allow for calling attachInterrupt(pin, NULL, mode), we set up all needed register
// but won't service the interrupt, this way we also don't need to check it inside the ISR.
if (callback)
{
  // Store interrupts to service in order of when they were attached
  // to allow for first come first serve handler
  uint32_t current = 0;

  // Check if we already have this interrupt
  for (current=0; current<nints; current++) {
    if (ISRlist[current] == inMask) {
      break;
    }
  }
  if (current == nints) {
    // Need to make a new entry
    nints++;
  }
  ISRlist[current] = inMask;       // List of interrupt in order of when they were attached
  ISRcallback[current] = callback; // List of callback adresses

  // Look for right CONFIG register to be addressed
  config = in / 8;
  pos = (in % 8) * 4;

  // Configure the interrupt mode
  // NOTE: RIGHT NOW BECAUSE NO CLOCK (STANDBY) - only HIGH or LOW.
  EIC->CONFIG[config].reg &=~ (EIC_CONFIG_SENSE0_Msk << pos); // Reset sense mode, important when changing trigger mode during runtime
  switch (mode)
  {
    case LOW:
      EIC->CONFIG[config].reg |= EIC_CONFIG_SENSE0_LOW_Val << pos;
      break;

    case HIGH:
      EIC->CONFIG[config].reg |= EIC_CONFIG_SENSE0_HIGH_Val << pos;
      break;

    default:
      return false;
  }
}
// Enable the interrupt
EIC->INTENSET.reg = EIC_INTENSET_EXTINT(inMask);
}



/*
 * External Interrupt Controller NVIC Interrupt Handler
 */
void EIC_Handler(void)
{
  // Interrupt flag is cleared by writing a 1 to it.
  EIC->INTFLAG.reg = inMask;
  // Calling the routine directly from -here- takes about 1us
  // Depending on where you are in the list it will take longer

  // Loop over all enabled interrupts in the list
  // for (uint32_t i=0; i<nints; i++)
  // {
  //   if ((EIC->INTFLAG.reg & ISRlist[i]) != 0)
  //   {
  //     // Call the callback function
  //     //ISRcallback[i]();
  //     // Clear the interrupt
  //     EIC->INTFLAG.reg = ISRlist[i];
  //   }
  // }
}


void Extint::standbyMode()
{
  // Entering standby mode when connected
  // via the native USB port causes issues.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}
