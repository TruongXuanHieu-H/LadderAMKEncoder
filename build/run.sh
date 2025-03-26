#!/bin/bash

n_values=$(seq 100 100 5000)

encoders=(--reduced --seq --BDD --card --scl)

w_values=(10 20 50)

# Timeout (600 seconds per command)
timeout_duration=600

# Memory limit (30000MB = 30000 * 1024 KB)
memory_limit=$((30000 * 1024))

for w in "${w_values[@]}"; do
    for encoder in "${encoders[@]}"; do
        for n in $n_values; do
            w_plus_1=$((w + 1))
            w_div_5=$((w / 5))

            ulimit -v $memory_limit
            
            echo "Running: timeout $timeout_duration ./ladder_amk_enc $n $encoder -set-lb $w -set-ub $w_plus_1 -set-at-most $w_div_5"
            timeout $timeout_duration ./ladder_amk_enc $n $encoder -set-lb $w -set-ub $w_plus_1 -set-at-most $w_div_5
            
            exit_code=$?
            if [ $exit_code -eq 124 ]; then
                echo "Command timed out after $timeout_duration seconds"
            elif [ $exit_code -eq 137 ]; then
                echo "Command was killed due to exceeding memory limit ($memory_limit KB)"
            fi
        done
    done
done
