#!/bin/sh

make run BUILDTYPE=Release TEST_CASE=heap_sort &&
    git add . && git commit -m "feat: heap sort testing results" && git push &&
    shutdown
