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
 * Type declarations
 *
 */

#define NUM_CALLS (8U) // How many subsequent APIs are called at maximum.
#define NUM_APIS (3U)  // How many APIs there are.


/*
 * Variable declarations
 *
 */

/* ... */


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
    for (int i = 0; i < NUM_CALLS; ++i) {
        int f_select = klee_choose(NUM_APIS);
        if (f_select == 0) {
            int state_1 = 0;
            klee_make_symbolic(&state_1, sizeof(state_1), "state 1");
            buggy_api_setState(state_1);
        }
        if (f_select == 1) {
            int state_2 = 0;
            klee_make_symbolic(&state_2, sizeof(state_2), "state 2");
            buggy_api_setState2(state_2);
        }
        if (f_select == 2) {
            buggy_api_run();
        }
    }

    return 0;
}
