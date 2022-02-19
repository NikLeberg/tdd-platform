/**
 * @file buggy_api.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Fuzzing the buggy_api library
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

#include <string.h>
#include <stdio.h>
// #include <fuzzer/FuzzedDataProvider.h>

#include "fuzzing.h"
#include "buggy_api/buggy_api.h"


/*
 * Private Functions
 *
 */

/* ... */


/*
 * LLVM libFuzzer Functions
 *
 */

/**
 * @brief Initialize fuzzing
 *
 * Gets called from libFuzzer once before fuzzing begins. Used to generate a
 * initial seed file of a valid code path.
 *
 * @param argc unused
 * @param argv unused
 * @retval 0
 */
int LLVMFuzzerInitialize(int *argc, char ***argv) {
    (void)argc;
    (void)argv;
    // Generate seed file with a defined valid codepath.
    FILE *fp = fopen("./seeds/valid-seed", "w");
    uint8_t api_function = 0;
    fwrite(&api_function, sizeof(uint8_t), 1, fp);
    uint32_t api_argument = 111; // should be 123 to trigger crash
    fwrite(&api_argument, sizeof(uint32_t), 1, fp);
    api_function = 1;
    fwrite(&api_function, sizeof(uint8_t), 1, fp);
    api_argument = 111; // should be 321 to trigger crash
    fwrite(&api_argument, sizeof(uint32_t), 1, fp);
    api_function = 2;
    fwrite(&api_function, sizeof(uint8_t), 1, fp);
    fclose(fp);
    return 0;
}

/**
 * @brief Main fuzzer
 *
 * Gets called from libFuzzer's main() and feeds data to the API under test.
 *
 * @param data input data from fuzzer
 * @param size length of data
 * @retval 0
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    fuzzer_setData(data, size);
    // Reset the API
    buggy_api_setState(0);
    buggy_api_setState2(0);
    // Select which function of the API to call. And then call the function with
    // the required arguments.
    while (fuzzer_remainingData()) {
        switch (fuzzer_consume_uint8_t()) {
        case 0: {
            buggy_api_setState(fuzzer_consume_uint32_t());
            break;
        }
        case 1: {
            buggy_api_setState2(fuzzer_consume_uint32_t());
            break;
        }
        case 2:
            buggy_api_run();
            break;
        default:
            return 0;
        }
    }
    return 0;
}
