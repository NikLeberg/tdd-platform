/**
 * @file i2c_extra.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Extra functions for i2c implementation of "linux"
 * @version 0.1
 * @date 2022-02-11
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

// These declarations are identical with the ones in i2c.h but also declare
// some other variables and structs needed for fff.
DECLARE_FAKE_VALUE_FUNC(int, i2c_init, int, int, int)
DECLARE_FAKE_VALUE_FUNC(int, i2c_deinit, int)
DECLARE_FAKE_VALUE_FUNC(int, i2c_send, int, uint8_t, uint8_t *, size_t)
DECLARE_FAKE_VALUE_FUNC(int, i2c_receive, int, uint8_t, uint8_t *, size_t)

/**
 * @brief Reset all fakes.
 *
 */
void i2c_extraResetFakes(void);
