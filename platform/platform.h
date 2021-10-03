/**
 * @file platform.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform abstraction
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * The platform implementation may provide following POSIX-apis:
 * - C-Standard:
 *      - <stdlib.h>
 *      - <stdbool.h>
 *      - <stddef.h>
 *      - <stdint.h>
 *      - <stdio.h>
 *      - <string.h>
 * - Threads & Semaphores:
 *      - <pthread.h>
 *      - <semaphore.h>
 * - Berkley Sockets:
 *      - <arpa/inet.h>
 *      - <net/if.h>
 *      - <netdb.h>
 *      - <netinet/in.h>
 *      - <netinet/tcp.h>
 *
 * Additionally GPIO and peripheral APIs according to the definitions in this
 * header may be provided.
 *
 * If a platform can't provide such functionality, the API has to be stubbed out
 * and must return sensible errors or trigger an assert.
 */

#pragma once


/*
 * Includes
 *
 */

/* ... */

/*
 * Type declarations
 *
 */

/**
 * @brief Direction modes for GPIO pin.
 *
 */
typedef enum {
    GPIO_DIRMODE_DONT_CHANGE, //!< Keep default or existing configuration.
    GPIO_DIRMODE_INPUT,       //!< Pin is a input.
    GPIO_DIRMODE_OUTPUT       //!< Pin is a output.
} gpio_dirMode_t;

/**
 * @brief Pullup & -down modes for GPIO pin.
 *
 */
typedef enum {
    GPIO_PULLMODE_DONT_CHANGE, //!< Keep default or existing configuration.
    GPIO_PULLMODE_UP,          //!< Enable only pullup resistor.
    GPIO_PULLMODE_DOWN,        //!< Enable only pulldown resistor.
    GPIO_PULLMODE_UP_DOWN,     //!< Enable pullup and -down resistor.
    GPIO_PULLMODE_FLOATING     //!< Disable both pull resistors.
} gpio_pullMode_t;

/**
 * @brief Type of triggers for gpio interrupts.
 *
 */
typedef enum {
    GPIO_INT_TRIGGER_DONT_CHANGE,   //!< Keep default or existing configuration.
    GPIO_INT_TRIGGER_DISABLED,      //!< Disable interrupt.
    GPIO_INT_TRIGGER_POSITIVE_EDGE, //!< On rising edge of signal level.
    GPIO_INT_TRIGGER_NEGATIVE_EDGE, //!< On falling edge of signal level.
    GPIO_INT_TRIGGER_ANY_EDGE,      //!< On all signal level changes.
    GPIO_INT_TRIGGER_LOW_LEVEL,     //!< While signal is low. (continuous!)
    GPIO_INT_TRIGGER_HIGH_LEVEL,    //!< While signal is high. (continuous!)
} gpio_interruptTrigger_t;

/**
 * @brief Prototype of interrupt callback.
 *
 */
typedef void (*gpio_interruptCallback_t)(void *);


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
 * @brief Setup common GPIO pin settings.
 *
 * @note Every setting accepts *_DONT_CHANGE whitch ignores the setting e.g.
 * keeps the default or an already existing setting.
 *
 * @param pin Number of the GPIO pin to setup.
 * @param dirMode Direction of dataflow.
 * @param pullMode Pullup & -down resistors.
 * @return int 0 on success EINVAL, ENOTSUP or EIO on error.
 */
int gpio_setupPin(int pin, gpio_dirMode_t dirMode, gpio_pullMode_t pullMode);

/**
 * @brief Get current level of GPIO pin.
 *
 * @param pin Number of the GPIO pin.
 * @return int 0 = Low, 1 = High, -EIO = error
 */
int gpio_getLevel(int pin);

/**
 * @brief Set level of GPIO pin.
 *
 * @param pin Number of the GPIO pin.
 * @param level Level to set: 0 = Low, 1 = High
 * @return int 0 on success or EIO on error.
 */
int gpio_setLevel(int pin, int level);

/**
 * @brief Setup a callback that gets called on the configured trigger.
 *
 * @warning Not all pins can be configured for interrupts.
 * @note To disable interrupts on pin, call with \arg trigger = \ref
 * GPIO_INT_TRIGGER_DISABLED.
 *
 * @param pin Number of the GPIO pin that triggers the interrupt.
 * @param trigger Trigger for the interrupt.
 * @param callback Callback function that is executed on interrupt.
 * @param arg Optional argument for callback.
 * @return int 0 on success, EIO on I/O error or ENOTSUP if pin does not
 * support interrupts.
 */
int gpio_setupInterrupt(int pin, gpio_interruptTrigger_t trigger,
                        gpio_interruptCallback_t callback, void *arg);

/**
 * @brief Main entrypoint of the application.
 *
 * @return int errorlevel
 */
int application_main();
