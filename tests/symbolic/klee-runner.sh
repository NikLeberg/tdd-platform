#!/bin/env bash

# This script intends to continuously provide stats while running klee.
# 
# Args:
# 1 - klee executable
# 2 - klee-stats executable
# 3 - llvm bytecode to execute

$1 --entry-point=application_main --only-output-states-covering-new --optimize $3 &
PROC_ID=$!

while kill -0 "$PROC_ID" >/dev/null 2>&1; do
    sleep 5
    $2 --print-more ./klee-last
done
exit 0
