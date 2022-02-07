/**
 * @file platform.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform abstraction
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * The platform implementation is implicitly required to provide following
 * C-Standard functionality:
 * - Types - bool, uint8_t, uint16_t, ...
 * - Memory allocation - malloc, calloc, realloc, free
 * - Standard-I/O - printf
 * - Math - abs, exp, sin, cos, ...
 * - Errnos - EINVAL, ENOTSUP, EIO, ...
 *
 * Additional functionality may be implemented according to the function and
 * type declarations of this header file and the header files it includes.
 * These additional functionalities cover following:
 * - Multitasking - tasks.h
 * - General Purpose Input Output - gpio.h
 * - Peripherals:
 *      - I2C - i2c.h
 *      - UART - uart.h
 * If a platform implementation supports such functionality the corresponding
 * CMake option PLATFORM_SUPPORTS_* has to be enabled.
 */

#pragma once


/*
 * Includes
 *
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#ifdef PLATFORM_SUPPORTS_TASKS
#include "task.h"
#endif
#ifdef PLATFORM_SUPPORTS_GPIO
#include "gpio.h"
#endif
#ifdef PLATFORM_SUPPORTS_PERIPH_I2C
#include "i2c.h"
#endif
#ifdef PLATFORM_SUPPORTS_PERIPH_UART
#include "uart.h"
#endif

/*
 * Type declarations
 *
 */

/* ... */


/*
 * Variable declarations
 *
 */

/* ... */


/*
 * Public functions
 *
 */

/**
 * @brief Main entrypoint of the application.
 *
 * @return int errorlevel
 */
int application_main();
