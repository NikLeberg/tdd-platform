/**
 * @file platform.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Platform implementation for "linux"
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

#include <errno.h>

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
 * @brief libc entry point.
 *
 * Somewhere in the libc implementation of gcc this function is called.
 * Simply call the application_main() of the main application.
 *
 */
int main() {
    // Call the main application.
    application_main();
}
