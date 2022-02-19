/**
 * @file test.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Common header for tests with cmocka.
 * @version 0.1
 * @date 2021-07-12
 *
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 *
 */

#pragma once


/*
 * Includes
 *
 */

#include <string.h>
#include <stdalign.h>

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

/**
 * @brief Current fuzzer input buffer.
 *
 */
static struct {
    const uint8_t *data;
    size_t size;
} fuzz;


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
 * @brief Initialize fuzzer data buffer.
 *
 * Needs to be called at the beginning of LLVMFuzzerTestOneInput() before
 * calling on of the other fuzzer_* functions.
 *
 * @param data argument of LLVMFuzzerTestOneInput
 * @param size argument of LLVMFuzzerTestOneInput
 */
void fuzzer_setData(const uint8_t *data, size_t size) {
    fuzz.data = data;
    fuzz.size = size;
}

/**
 * @brief Get remaining size of data in bytes.
 *
 * @return size_t remaining data size in bytes
 */
size_t fuzzer_remainingData(void) {
    return fuzz.size;
}

/**
 * @brief Get a raw pointer into the fuzzer data buffer.
 *
 * This allows to consume data from the buffer in smaller chunks.
 * @warning Do not dereference the returned pointer directly to the required
 * data type (e.g. int or uint64_t). Doing so triggers UBSAN with a warning on
 * unaligned pointer read. Use memcpy() to copy the contents or access it byte
 * by byte.
 *
 * @param size size in bytes
 * @return void* pointer into fuzzer buffer
 */
void *fuzzer_consumeData(size_t size) {
    void *data = (void *)fuzz.data;
    fuzz.data += size;
    fuzz.size -= size;
    return data;
}

/**
 * @brief Implement a function that returns the given data type.
 *
 * Insted of using the upper fuzzer_consumeData() and memcpy'ing the data this
 * defines a helper function with the name fuzzer_consume_<T>() that returns a
 * value from the fuzzer buffer with the "right" type.
 * @note The implemented function returns 0 when the fuzzer buffer has not
 * enought data.
 */
#define FUZZER_IMPLEMENT_CONSUME(T)                                            \
    T fuzzer_consume_##T() {                                                   \
        T value = 0;                                                           \
        if (fuzz.size >= sizeof(T)) {                                          \
            memcpy(&value, fuzzer_consumeData(sizeof(T)), sizeof(T));          \
        } else {                                                               \
            fuzz.size = 0;                                                     \
        }                                                                      \
        return value;                                                          \
    }

/**
 * @brief Implement the fuzzer_consume functions for common types.
 *
 */
FUZZER_IMPLEMENT_CONSUME(uint8_t)
FUZZER_IMPLEMENT_CONSUME(int8_t)
FUZZER_IMPLEMENT_CONSUME(uint16_t)
FUZZER_IMPLEMENT_CONSUME(int16_t)
FUZZER_IMPLEMENT_CONSUME(uint32_t)
FUZZER_IMPLEMENT_CONSUME(int32_t)
FUZZER_IMPLEMENT_CONSUME(uint64_t)
FUZZER_IMPLEMENT_CONSUME(int64_t)
FUZZER_IMPLEMENT_CONSUME(float)
FUZZER_IMPLEMENT_CONSUME(double)

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
