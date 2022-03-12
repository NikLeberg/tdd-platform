/**
 * @file buggy_api.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Testing the buggy_api library with symbolic execution.
 * @version 0.1
 * @date 2022-03-12
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */


/*
 * Includes
 *
 */

#include "symbolic.h"
#include "buggy_api/buggy_api.h"


/*
 * Private Functions
 *
 */

/* ... */


/*
 * Main test program
 *
 */

/**
 * @brief Main application
 *
 * @return int always 0
 */
int application_main(void) {
    int state_1 = 0;
    klee_make_symbolic(&state_1, sizeof(state_1), "state 1");
    buggy_api_setState(state_1);
    int state_2 = 0;
    klee_make_symbolic(&state_2, sizeof(state_2), "state 2");
    buggy_api_setState2(state_2);
    buggy_api_run();

    return 0;
}
