/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform implementation for "esp8266"
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

#include "platform.h"


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
 * Simply call the main() of the main application.
 *
 */
void app_main() {
    // Call the main application.
    main();
}
