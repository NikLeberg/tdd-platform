/**
 * @file i2c.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief i2c implementation for "linux"
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * As most linux PC's do not have I2C bus hardware to work with all functions
 * are only fakes / mocks. They are generated with the Fake Function Framework
 * (fff). The fakes can be controlled through the specifics of fff and with some
 * extra functions in \ref gpio_extra.h. The extra header has to be included
 * i.e. #include "platform/linux/i2c_extra.h" to use and control these
 * functions. Otherwise they are just stubs that do nothing.
 */


/*
 * Includes
 *
 */

#include "i2c_extra.h"

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
    FAKE(i2c_init)                                                             \
    FAKE(i2c_deinit)                                                           \
    FAKE(i2c_send)                                                             \
    FAKE(i2c_receive)


/*
 * Private function prototypes
 *
 */

/* ... */


/*
 * Implementation of public functions
 *
 */

DEFINE_FAKE_VALUE_FUNC(int, i2c_init, int, int, int, int)
DEFINE_FAKE_VALUE_FUNC(int, i2c_deinit, int)
DEFINE_FAKE_VALUE_FUNC(int, i2c_send, int, uint8_t, uint8_t *, size_t, uint32_t)
DEFINE_FAKE_VALUE_FUNC(int, i2c_receive, int, uint8_t, uint8_t *, size_t,
                       uint32_t)

void i2c_extraResetFakes(void) {
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}


/*
 * Implementation of private functions
 *
 */

/* ... */
