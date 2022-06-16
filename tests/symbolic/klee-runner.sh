#!/bin/env bash

# This script intends to continuously provide stats while running klee. And
# after running analyzing the produced tests that crashed the program.
# 
# Args:
# 1 - klee executable
# 2 - klee-stats executable
# 3 - ktest-tool executable
# 4 - llvm bytecode to execute

# Run klee itself in the background.
$1 --entry-point=application_main --exit-on-error \
    --only-output-states-covering-new --optimize $4 &
PROC_ID=$!

# As long as klee is running, print stats every 5 seconds.
while kill -0 "$PROC_ID" >/dev/null 2>&1; do
    sleep 5
    $2 --print-more ./klee-last
done

# Print out ktest content for failing tests.
files=($(ls ./klee-last/*err))
for file in "${files[@]}" ; do
    # Remove file extension of error message file and run through ktest tool.
    $3 ${file%.*.*}.ktest
done

exit 0