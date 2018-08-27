/**
 * \file
 *
 * \brief EIC related functionality implementation.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * I modified this to understand..
 *
 */
#include <compiler.h>
#include <hpl_eic_config.h>
#include <hpl_ext_irq.h>
#include <string.h>
#include <utils.h>
#include <utils_assert.h>
// Added...
#include "atmel_start_pins.h"

#ifdef __MINGW32__
#define ffs __builtin_ffs
#endif
#if defined(__CC_ARM) || defined(__ICCARM__)
/* Find the first bit set */
static int ffs(int v)
{
	int i, bit = 1;
	for (i = 0; i < sizeof(int) * 8; i++) {
		if (v & bit) {
			return i + 1;
		}
		bit <<= 1;
	}
	return 0;
}
#endif

/**
 * \brief Invalid external interrupt and pin numbers
 */
#define INVALID_EXTINT_NUMBER 0xFF
#define INVALID_PIN_NUMBER 0xFFFFFFFF

#ifndef CONFIG_EIC_EXTINT_MAP
/** Dummy mapping to pass compiling. */
#define CONFIG_EIC_EXTINT_MAP                                                                                          \
	{                                                                                                                  \
		INVALID_EXTINT_NUMBER, INVALID_PIN_NUMBER                                                                      \
	}
#endif

#define EXT_IRQ_AMOUNT 1

/**
 * \brief EXTINTx and pin number map
 */
struct _eic_map {
	uint8_t  extint;
	uint32_t pin;
};

/**
 * \brief PIN and EXTINT map for enabled external interrupts
 */
static const struct _eic_map _map[] = {CONFIG_EIC_EXTINT_MAP};

/**
 * \brief The callback to upper layer's interrupt processing routine
 */
static void (*callback)(const uint32_t pin);

/**
 * \brief Initialize external interrupt module
 */
int32_t _ext_irq_init(void (*cb)(const uint32_t pin))
{
	hri_eic_wait_for_sync(EIC);
	// #define EIC_CTRL_ENABLE_Pos         1            /**< \brief (EIC_CTRL) Enable */
	// #define EIC_CTRL_ENABLE             (_U_(0x1) << EIC_CTRL_ENABLE_Pos)
	// If the EIC module is running...
	if (hri_eic_get_CTRL_reg(EIC, EIC_CTRL_ENABLE)) {
		// Turn it off.
		hri_eic_write_CTRL_reg(EIC, 0);
		hri_eic_wait_for_sync(EIC);
	}
	//Reset the EIC module by setting the Software Reset bit
	hri_eic_write_CTRL_reg(EIC, EIC_CTRL_SWRST);
	hri_eic_wait_for_sync(EIC);

	// hri_eic_write_NMICTRL_reg(
	//     EIC, (CONF_EIC_NMIFILTEN << EIC_NMICTRL_NMIFILTEN_Pos) | EIC_NMICTRL_NMISENSE(CONF_EIC_NMISENSE));
	// hri_eic_write_EVCTRL_reg(EIC,
	//                          (CONF_EIC_EXTINTEO0 << 0) | (CONF_EIC_EXTINTEO1 << 1) | (CONF_EIC_EXTINTEO2 << 2)
	//                              | (CONF_EIC_EXTINTEO3 << 3) | (CONF_EIC_EXTINTEO4 << 4) | (CONF_EIC_EXTINTEO5 << 5)
	//                              | (CONF_EIC_EXTINTEO6 << 6) | (CONF_EIC_EXTINTEO7 << 7) | (CONF_EIC_EXTINTEO8 << 8)
	//                              | (CONF_EIC_EXTINTEO9 << 9) | (CONF_EIC_EXTINTEO10 << 10) | (CONF_EIC_EXTINTEO11 << 11)
	//                              | (CONF_EIC_EXTINTEO12 << 12) | (CONF_EIC_EXTINTEO13 << 13)
	//                              | (CONF_EIC_EXTINTEO14 << 14) | (CONF_EIC_EXTINTEO15 << 15) | 0);

	hri_eic_write_WAKEUP_reg(EIC,
	                         (CONF_EIC_WAKEUPEN0 << 0) | (CONF_EIC_WAKEUPEN1 << 1) | (CONF_EIC_WAKEUPEN2 << 2)
	                             | (CONF_EIC_WAKEUPEN3 << 3) | (CONF_EIC_WAKEUPEN4 << 4) | (CONF_EIC_WAKEUPEN5 << 5)
	                             | (CONF_EIC_WAKEUPEN6 << 6) | (CONF_EIC_WAKEUPEN7 << 7) | (CONF_EIC_WAKEUPEN8 << 8)
	                             | (CONF_EIC_WAKEUPEN9 << 9) | (CONF_EIC_WAKEUPEN10 << 10) | (CONF_EIC_WAKEUPEN11 << 11)
	                             | (CONF_EIC_WAKEUPEN12 << 12) | (CONF_EIC_WAKEUPEN13 << 13)
	                             | (CONF_EIC_WAKEUPEN14 << 14) | (CONF_EIC_WAKEUPEN15 << 15) | 0);
	hri_eic_write_CONFIG_reg(EIC,
	                         0,
	                         (CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
	                             | (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
	                             | (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
	                             | (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE3)
	                             | (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
	                             | (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
	                             | (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
	                             | (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
	                             | 0);

	hri_eic_write_CONFIG_reg(EIC,
	                         1,
	                         (CONF_EIC_FILTEN8 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE8)
	                             | (CONF_EIC_FILTEN9 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE9)
	                             | (CONF_EIC_FILTEN10 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE10)
	                             | (CONF_EIC_FILTEN11 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE11)
	                             | (CONF_EIC_FILTEN12 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE12)
	                             | (CONF_EIC_FILTEN13 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE13)
	                             | (CONF_EIC_FILTEN14 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE14)
	                             | (CONF_EIC_FILTEN15 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE15)
	                             | 0);
  // Turn on the EIC module by writing a '1' in the enable bit of the CTRL register.
	hri_eic_set_CTRL_ENABLE_bit(EIC);
  // Tell NVIC to let traffic from the EIC module through.
	NVIC_DisableIRQ(EIC_IRQn);
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_EnableIRQ(EIC_IRQn);

	callback = cb;

	return ERR_NONE;
}

/**
 * \brief De-initialize external interrupt module
 */
int32_t _ext_irq_deinit(void)
{
	NVIC_DisableIRQ(EIC_IRQn);
	callback = NULL;

	hri_eic_clear_CTRL_ENABLE_bit(EIC);
	hri_eic_set_CTRL_SWRST_bit(EIC);

	return ERR_NONE;
}

/**
 * \brief Enable / disable external irq
 */
int32_t _ext_irq_enable(const uint32_t pin, const bool enable)
{
	uint8_t extint = INVALID_EXTINT_NUMBER;
	uint8_t i      = 0;

	for (; i < ARRAY_SIZE(_map); i++) {
		if (_map[i].pin == pin) {
			extint = _map[i].extint;
			break;
		}
	}
	if (INVALID_EXTINT_NUMBER == extint) {
		return ERR_INVALID_ARG;
	}

	if (enable) {
		hri_eic_set_INTEN_reg(EIC, 1ul << extint);
	} else {
		hri_eic_clear_INTEN_reg(EIC, 1ul << extint);
		hri_eic_clear_INTFLAG_reg(EIC, 1ul << extint);
	}

	return ERR_NONE;
}

/**
 * \brief EIC interrupt handler
 * THIS IS GENERICALLY MESSY...then I had a problem because flags is reset...
 * which seems to put this thing in a never ending loop.
 */
void EIC_Handler(void)
{
	volatile uint32_t flags = hri_eic_read_INTFLAG_reg(EIC) & hri_eic_read_INTEN_reg(EIC);
	//int8_t            pos;
	//uint32_t          pin = INVALID_PIN_NUMBER;
  // Clear the interrupt flag.
	
	hri_eic_clear_INTFLAG_reg(EIC, flags);

	ASSERT(callback);
	// pin turns out to be 19

	callback(PA19);

	// while (flags) {
	// 	pos = ffs(flags) - 1;
	// 	while (-1 != pos) {
	// 		uint8_t lower = 0, middle, upper = EXT_IRQ_AMOUNT;
	//
	// 		while (upper >= lower) {
	// 			middle = (upper + lower) >> 1;
	// 			if (_map[middle].extint == pos) {
	// 				pin = _map[middle].pin;
	// 				break;
	// 			}
	// 			if (_map[middle].extint < pos) {
	// 				lower = middle + 1;
	// 			} else {
	// 				upper = middle - 1;
	// 			}
	// 		}

		// 	if (INVALID_PIN_NUMBER != pin) {
		// 		callback(pin);
		// 	}
		// 	flags &= ~(1ul << pos);
		// 	pos = ffs(flags) - 1;
		// }

	// }
}
