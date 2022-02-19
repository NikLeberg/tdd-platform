/**
 * @file buggy_api.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Example library that has some bugs.
 * @version 0.1
 * @date 2022-02-17
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include <stdio.h>
#include <assert.h>

#include "buggy_api/buggy_api.h"


/*
 * Type declarations
 *
 */

/* ... */


/*
 * Variable declarations
 *
 */

static int g_state;
static int g_state2;


/*
 * Private function prototypes
 *
 */

/* ... */


/*
 * Implementation of public functions
 *
 */

void buggy_api_setState(int state) {
    // set global state
    g_state = state;
}

void buggy_api_setState2(int state) {
    // set global state
    g_state2 = state;
}

void buggy_api_run(void) {
    if (g_state == 123) {
        if (g_state2 == 321) {
            assert(0);
        }
    }
}


/*
 * Implementation of private functions
 *
 */

/* ... */
