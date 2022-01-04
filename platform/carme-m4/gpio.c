/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief GPIO implementation for "carme-m4"
 * @version 0.1
 * @date 2021-10-03
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include "stm32f4xx.h" // Processor is STM32F407IG
#include "carme.h"     // CARME Module

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
    (void)pin;
    (void)dirMode;
    (void)pullMode;
    return 0;
}

int gpio_getLevel(int pin) {
    (void)pin;
    return 0;
}

int gpio_setLevel(int pin, int level) {
    (void)pin;
    (void)level;
    return 0;
}

int gpio_setupInterrupt(int pin, gpio_interruptTrigger_t trigger,
                        gpio_interruptCallback_t callback, void *arg) {
    (void)pin;
    (void)trigger;
    (void)callback;
    (void)arg;
    return 0;
}


/*
 * Implementation of private functions
 *
 */

/* ... */
