/*! \file *********************************************************************
 *
 * \brief  ATtiny88 External interrupts driver source file.
 *
 *      This file contains the function implementations the external interrupts
 *      driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      some functions are several lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the ATtiny88 external interrupts.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1201: Using external interrupts for tinyAVR devices
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * Copyright (c) 2011, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include "ext_interrupt_driver.h"

/*! \brief This function configures the external interrupt to sense either
 *         rising edge, falling edge, both the edges or low level to
 *         rise an interrupt request.
 *  \param INT_NO	The interrupt for which configuration has to be setup.
 *  \param INT_MODE	The sensing mode of the INT_NO.
 */

void Configure_Interrupt(uint8_t INT_NO, uint8_t INT_MODE)
{
	switch (INT_NO) {
	case 0:
		switch (INT_MODE) {
		case 0:
			EICRA = (EICRA & (~(1 << ISC01 | 1 << ISC00))) | (0 << ISC01 | 0 << ISC00);
			break;
		case 1:
			EICRA = (EICRA & (~(1 << ISC01 | 1 << ISC00))) | (0 << ISC01 | 1 << ISC00);
			break;
		case 2:
			EICRA = (EICRA & (~(1 << ISC01 | 1 << ISC00))) | (1 << ISC01 | 0 << ISC00);
			break;
		case 3:
			EICRA = (EICRA & (~(1 << ISC01 | 1 << ISC00))) | (1 << ISC01 | 1 << ISC00);
			break;
		default:
			break;
		}
		break;

	case 1:
		switch (INT_MODE) {
		case 0:
			EICRA = (EICRA & (~(1 << ISC11 | 1 << ISC10))) | (0 << ISC11 | 0 << ISC10);
			break;
		case 1:
			EICRA = (EICRA & (~(1 << ISC11 | 1 << ISC10))) | (0 << ISC11 | 1 << ISC10);
			break;
		case 2:
			EICRA = (EICRA & (~(1 << ISC11 | 1 << ISC10))) | (1 << ISC11 | 0 << ISC10);
			break;
		case 3:
			EICRA = (EICRA & (~(1 << ISC11 | 1 << ISC10))) | (1 << ISC11 | 1 << ISC10);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

/*! \brief This function enables the external interrupt.
 *
 *  \param INT_NO	The interrupt which has to be enabled.
 */
void Enable_Interrupt(uint8_t INT_NO)
{
	switch (INT_NO) {
	case 0:
		EIMSK |= (1 << INT0);
		break;
	case 1:
		EIMSK |= (1 << INT1);
		break;
	default:
		break;
	}
}

/*! \brief This function enables the external pin change interrupt.
 *
 *  \param PCINT_NO	The pin change interrupt which has to be enabled.
 */
void Enable_Pcinterrupt(uint8_t PCINT_NO)
{
	if (PCINT_NO >= 0 && PCINT_NO <= 7) {
		PCICR = (PCICR & (~(1 << PCIE0))) | (1 << PCIE0);

		switch (PCINT_NO) {
		case 0:
			PCMSK0 |= (1 << PCINT0);
			break;
		case 1:
			PCMSK0 |= (1 << PCINT1);
			break;
		case 2:
			PCMSK0 |= (1 << PCINT2);
			break;
		case 3:
			PCMSK0 |= (1 << PCINT3);
			break;
		case 4:
			PCMSK0 |= (1 << PCINT4);
			break;
		case 5:
			PCMSK0 |= (1 << PCINT5);
			break;
		case 6:
			PCMSK0 |= (1 << PCINT6);
			break;
		case 7:
			PCMSK0 |= (1 << PCINT7);
			break;
		default:
			break;
		}
	} else if (PCINT_NO >= 8 && PCINT_NO <= 15) {
		PCICR = (PCICR & (~(1 << PCIE1))) | (1 << PCIE1);

		switch (PCINT_NO) {
		case 8:
			PCMSK1 |= (1 << PCINT8);
			break;
		case 9:
			PCMSK1 |= (1 << PCINT9);
			break;
		case 10:
			PCMSK1 |= (1 << PCINT10);
			break;
		case 11:
			PCMSK1 |= (1 << PCINT11);
			break;
		case 12:
			PCMSK1 |= (1 << PCINT12);
			break;
		case 13:
			PCMSK1 |= (1 << PCINT13);
			break;
		case 14:
			PCMSK1 |= (1 << PCINT14);
			break;
		case 15:
			PCMSK1 |= (1 << PCINT15);
			break;
		default:
			break;
		}
	} else if (PCINT_NO >= 16 && PCINT_NO <= 23) {
		PCICR = (PCICR & (~(1 << PCIE2))) | (1 << PCIE2);

		switch (PCINT_NO) {
		case 16:
			PCMSK2 |= (1 << PCINT16);
			break;
		case 17:
			PCMSK2 |= (1 << PCINT17);
			break;
		case 18:
			PCMSK2 |= (1 << PCINT18);
			break;
		case 19:
			PCMSK2 |= (1 << PCINT19);
			break;
		case 20:
			PCMSK2 |= (1 << PCINT20);
			break;
		case 21:
			PCMSK2 |= (1 << PCINT21);
			break;
		case 22:
			PCMSK2 |= (1 << PCINT22);
			break;
		case 23:
			PCMSK2 |= (1 << PCINT23);
			break;
		default:
			break;
		}
	} else {
		PCICR = (PCICR & (~(1 << PCIE3))) | (1 << PCIE3);

		switch (PCINT_NO) {
		case 24:
			PCMSK3 |= (1 << PCINT24);
			break;
		case 25:
			PCMSK3 |= (1 << PCINT25);
			break;
		case 26:
			PCMSK3 |= (1 << PCINT26);
			break;
		case 27:
			PCMSK3 |= (1 << PCINT27);
			break;
		default:
			break;
		}
	}
}

/*! \brief This function disables the external interrupt.
 *
 *  \param INT_NO	The interrupt which has to be disabled.
 */
void Disable_Interrupt(uint8_t INT_NO)
{
	switch (INT_NO) {
	case 0:
		EIMSK = (EIMSK & (~(1 << INT0)));
		break;
	case 1:
		EIMSK = (EIMSK & (~(1 << INT1)));
		break;
	default:
		break;
	}
}

/*! \brief This function disables the external pin change interrupt.
 *
 *  \param PCINT_NO	The pin change interrupt which has to be disabled.
 */
void Disable_Pcinterrupt(uint8_t PCINT_NO)
{
	switch (PCINT_NO) {
	case 0:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT0)));
		break;
	case 1:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT1)));
		break;
	case 2:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT2)));
		break;
	case 3:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT3)));
		break;
	case 4:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT4)));
		break;
	case 5:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT5)));
		break;
	case 6:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT6)));
		break;
	case 7:
		PCMSK0 = (PCMSK0 & (~(1 << PCINT7)));
		break;
	case 8:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT8)));
		break;
	case 9:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT9)));
		break;
	case 10:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT10)));
		break;
	case 11:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT11)));
		break;
	case 12:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT12)));
		break;
	case 13:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT13)));
		break;
	case 14:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT14)));
		break;
	case 15:
		PCMSK1 = (PCMSK1 & (~(1 << PCINT15)));
		break;
	case 16:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT16)));
		break;
	case 17:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT17)));
		break;
	case 18:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT18)));
		break;
	case 19:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT19)));
		break;
	case 20:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT20)));
		break;
	case 21:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT21)));
		break;
	case 22:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT22)));
		break;
	case 23:
		PCMSK2 = (PCMSK2 & (~(1 << PCINT23)));
		break;
	case 24:
		PCMSK3 = (PCMSK3 & (~(1 << PCINT24)));
		break;
	case 25:
		PCMSK3 = (PCMSK3 & (~(1 << PCINT25)));
		break;
	case 26:
		PCMSK3 = (PCMSK3 & (~(1 << PCINT26)));
		break;
	case 27:
		PCMSK3 = (PCMSK3 & (~(1 << PCINT27)));
		break;
	default:
		break;
	}

	if (PCMSK0 == 0x00) {
		PCICR = (PCICR & (~(1 << PCIE0)));
	} else if (PCMSK1 == 0x00) {
		PCICR = (PCICR & (~(1 << PCIE1)));
	} else if (PCMSK2 == 0x00) {
		PCICR = (PCICR & (~(1 << PCIE2)));
	} else if (PCMSK3 == 0x00) {
		PCICR = (PCICR & (~(1 << PCIE3)));
	}
}
