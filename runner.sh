#!/bin/sh

make run BUILDTYPE=Release TEST_CASE=bin_heap_sort &&
    git add . && git commit -m "feat: binary heap sort testing results" && git push &&
    shutdown
