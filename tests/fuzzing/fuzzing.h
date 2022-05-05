/**
 * @file fuzzing.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Common header for fuzzers.
 * @version 0.1
 * @date 2022-02-18
 *
 * @copyright Copyright (c) 2022 Niklaus Leuenberger
 *
 */

#pragma once


/*
 * Includes
 *
 */

#include "platform/platform.h"


/*
 * Type declarations
 *
 */

/**
 * @brief Dummy global context for modelling global APIs with GraphFuzz.
 *
 */
typedef struct {
    int dummy;
} globalContext;


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
 * @brief Dummy platform library entry point.
 *
 * This function is never called. Normally the platform abstraction library
 * defines the main() function and calls this function after initialization. But
 * when fuzzing with libFuzzer from LLVM it provides its own main() which
 * overwrites the platform-main().
 *
 * @return int 0
 */
int application_main(void) {
    return 0;
}

/**
 * @brief Default options for sanitizers.
 *
 * The fuzzer gets compiled and linked with ASAN, LSAN and UBSAN.
 * These options change the default configuration of the saniziters.
 *
 * @return const char* options, separated with a colon
 */
const char *__asan_default_options() {
    return "detect_stack_use_after_return=true:"
           "strict_string_checks=true";
}
// const char *__lsan_default_options() {
//     return "log_threads=1:"
//            "verbosity=1";
// }
const char *__ubsan_default_options() {
    return "report_error_type=true";
}
