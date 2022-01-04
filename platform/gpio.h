/**
 * @file gpio.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief GPIO abstraction
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
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
typedef enum gpio_dirMode_e {
    GPIO_DIRMODE_DONT_CHANGE, //!< Keep default or existing configuration.
    GPIO_DIRMODE_INPUT,       //!< Pin is a input.
    GPIO_DIRMODE_OUTPUT       //!< Pin is a output.
} gpio_dirMode_t;

/**
 * @brief Pullup & -down modes for GPIO pin.
 *
 */
typedef enum gpio_pullMode_e {
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
typedef enum gpio_interruptTrigger_e {
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
 * @retval 0 on success
 * @retval EINVAL on invalid parameters
 * @retval ENOTSUP if choosen \par dirMode or \par pullMode is not supported
 * @retval EIO on gpio error
 */
int gpio_setupPin(int pin, gpio_dirMode_t dirMode, gpio_pullMode_t pullMode);

/**
 * @brief Get current level of GPIO pin.
 *
 * @param pin Number of the GPIO pin.
 * @retval 0 = Low
 * @retval 1 = High
 * @retval -EIO = error
 */
int gpio_getLevel(int pin);

/**
 * @brief Set level of GPIO pin.
 *
 * @param pin Number of the GPIO pin.
 * @param level Level to set: 0 = Low, 1 = High
 * @retval 0 on success
 * @retval EIO on gpio error
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
 * @retval 0 on success
 * @retval ENOTSUP if pin does not support interrupts
 * @retval EIO on gpio error
 */
int gpio_setupInterrupt(int pin, gpio_interruptTrigger_t trigger,
                        gpio_interruptCallback_t callback, void *arg);
