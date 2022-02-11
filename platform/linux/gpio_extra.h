/**
 * @file gpio_extra.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Extra functions for GPIO implementation of "linux"
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */

#pragma once


/*
 * Includes
 *
 */

#include "fff.h"

#include "platform/platform.h"


/*
 * Type declarations
 *
 */

/* ... */


/*
 * Variable declarations
 *
 */

/**
 * @brief Globals for fff.
 *
 */
extern DEFINE_FFF_GLOBALS;


/*
 * Public functions
 *
 */

// These declarations are identical with the ones in gpio.h but also declare
// some other variables and structs needed for fff.
DECLARE_FAKE_VALUE_FUNC(int, gpio_setupPin, int, gpio_dirMode_t,
                        gpio_pullMode_t)
DECLARE_FAKE_VALUE_FUNC(int, gpio_getLevel, int)
DECLARE_FAKE_VALUE_FUNC(int, gpio_setLevel, int, int)
DECLARE_FAKE_VALUE_FUNC(int, gpio_setupInterrupt, int, gpio_interruptTrigger_t,
                        gpio_interruptCallback_t, void *)

/**
 * @brief Reset all fakes.
 *
 */
void gpio_extraResetFakes(void);
