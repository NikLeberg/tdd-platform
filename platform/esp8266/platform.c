/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform implementation for "esp8266"
 * @version 0.1
 * @date 2021-07-15
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 * Mostly the same as platform implementation for the esp32. Sadly there are a
 * few differences between the esp-idf implementations that does not allow for a
 * complete reuse of the code.
 *
 */


/*
 * Includes
 *
 */

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

/* ... */


/*
 * Implementation of private functions
 *
 */

/**
 * @brief esp-idf entry point.
 *
 * Somewhere in the FreeRTOS implementation of esp-idf this function is called.
 * Simply call the application_main() of the main application.
 *
 */
void app_main() {
    // Call the main application.
    application_main();
}
