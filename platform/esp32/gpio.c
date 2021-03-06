/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief GPIO implementation for "esp32"
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include "driver/gpio.h"

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

/* ... */


/*
 * Private function prototypes
 *
 */

/* ... */


/*
 * Implementation of public functions
 *
 */

int gpio_setupPin(int pin, gpio_dirMode_t dirMode, gpio_pullMode_t pullMode) {
    assert(pin >= 0 && pin < GPIO_NUM_MAX);
    esp_err_t err = ESP_OK;
    switch (dirMode) {
    case (GPIO_DIRMODE_DONT_CHANGE):
        break;
    case (GPIO_DIRMODE_INPUT):
        err = gpio_set_direction(pin, GPIO_MODE_INPUT);
        break;
    case (GPIO_DIRMODE_OUTPUT):
        err = gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        break;
    default:
        assert(false);
    }
    if (ESP_OK != err) {
        return EIO;
    }
    switch (pullMode) {
    case (GPIO_PULLMODE_DONT_CHANGE):
        break;
    case (GPIO_PULLMODE_UP):
        err = gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
        break;
    case (GPIO_PULLMODE_DOWN):
        err = gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
        break;
    case (GPIO_PULLMODE_UP_DOWN):
        err = gpio_set_pull_mode(pin, GPIO_PULLUP_PULLDOWN);
        break;
    case (GPIO_PULLMODE_FLOATING):
        err = gpio_set_pull_mode(pin, GPIO_FLOATING);
        break;
    default:
        assert(false);
    }
    if (ESP_OK != err) {
        return EIO;
    }
}

int gpio_getLevel(int pin) {
    assert(pin >= 0 && pin < GPIO_NUM_MAX);
    return gpio_get_level(pin);
}

int gpio_setLevel(int pin, int level) {
    assert(pin >= 0 && pin < GPIO_NUM_MAX);
    assert(level == 0 || level == 1);
    if (ESP_OK != gpio_set_level(pin, level)) {
        return EIO;
    }
    return 0;
}

int gpio_setupInterrupt(int pin, gpio_interruptTrigger_t trigger,
                        gpio_interruptCallback_t callback, void *arg) {
    assert(pin >= 0 && pin < GPIO_NUM_MAX);
    esp_err_t err = gpio_install_isr_service(0);
    if (ESP_OK != err && ESP_ERR_INVALID_STATE != err) {
        return EIO;
    }
    gpio_int_type_t intrType = GPIO_INTR_MAX;
    switch (trigger) {
    case (GPIO_INT_TRIGGER_DONT_CHANGE):
        break;
    case (GPIO_INT_TRIGGER_DISABLED):
        intrType = GPIO_INTR_DISABLE;
        break;
    case (GPIO_INT_TRIGGER_POSITIVE_EDGE):
        intrType = GPIO_INTR_POSEDGE;
        break;
    case (GPIO_INT_TRIGGER_NEGATIVE_EDGE):
        intrType = GPIO_INTR_NEGEDGE;
        break;
    case (GPIO_INT_TRIGGER_ANY_EDGE):
        intrType = GPIO_INTR_ANYEDGE;
        break;
    case (GPIO_INT_TRIGGER_LOW_LEVEL):
        intrType = GPIO_INTR_LOW_LEVEL;
        break;
    case (GPIO_INT_TRIGGER_HIGH_LEVEL):
        intrType = GPIO_INTR_HIGH_LEVEL;
        break;
    default:
        assert(false);
    }
    if (GPIO_INT_TRIGGER_DONT_CHANGE != trigger) {
        if (ESP_OK != gpio_set_intr_type(pin, intrType)) {
            return EIO;
        }
    }
    if (callback) {
        if (ESP_OK != gpio_isr_handler_add(pin, callback, arg)) {
            return EIO;
        }
    }
    if (ESP_OK != gpio_intr_enable(pin)) {
        return EIO;
    }
    return 0;
}


/*
 * Implementation of private functions
 *
 */

/* ... */
