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

extern "C" {
#include <stdio.h>
#include "fuzzing.h"
#include "buggy_api/buggy_api.h"
}

#include "buggy_api.pb.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <libfuzzer/libfuzzer_macro.h>


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
 * @brief Main fuzzer
 *
 * Gets called from libFuzzer's main() and feeds data to the API under test.
 * Instead of raw bytes a protobuf reperesentation according to a .proto file is
 * the input.
 *
 * @param input deserialized protobuf data from fuzzer
 */
DEFINE_PROTO_FUZZER(const Sequence &input) {
    // Reset the API
    buggy_api_setState(0);
    buggy_api_setState2(0);
    // Select which function of the API to call. And then call the function with
    // the required arguments.
    for (const auto &function : input.function()) {
        switch (function.function_case()) {
        case Function::kSetState:
            buggy_api_setState(function.setstate().state());
            break;
        case Function::kSetState2:
            buggy_api_setState2(function.setstate2().state());
            break;
        case Function::kRun:
            buggy_api_run();
            break;
        default:
            return;
        }
    }
    return;
}
