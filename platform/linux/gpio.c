/**
 * @file gpio.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief GPIO implementation for "linux"
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * As most linux PC's do not have GPIOs to work with all functions are only
 * fakes / mocks. They are generated with the Fake Function Framework (fff).
 * The fakes can be controlled through the specifics of fff and with some extra
 * functions in \ref gpio_extra.h. The extra header has to be included i.e.
 * #include "platform/linux/gpio_extra.h" to use and control these functions.
 * Otherwise they are just stubs that do nothing.
 */


/*
 * Includes
 *
 */

#include "gpio_extra.h"

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
 * @brief List of all fakes that are created.
 *
 */
#define FFF_FAKES_LIST(FAKE)                                                   \
    FAKE(gpio_setupPin)                                                        \
    FAKE(gpio_getLevel)                                                        \
    FAKE(gpio_setLevel)                                                        \
    FAKE(gpio_setupInterrupt)


/*
 * Private function prototypes
 *
 */

/* ... */


/*
 * Implementation of public functions
 *
 */

DEFINE_FAKE_VALUE_FUNC(int, gpio_setupPin, int, gpio_dirMode_t, gpio_pullMode_t)
DEFINE_FAKE_VALUE_FUNC(int, gpio_getLevel, int)
DEFINE_FAKE_VALUE_FUNC(int, gpio_setLevel, int, int)
DEFINE_FAKE_VALUE_FUNC(int, gpio_setupInterrupt, int, gpio_interruptTrigger_t,
                       gpio_interruptCallback_t, void *)

void gpio_extraResetFakes(void) {
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}


/*
 * Implementation of private functions
 *
 */

/* ... */
