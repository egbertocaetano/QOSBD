#!/bin/bash

set -eu

make

NUM_OF_PAGES=(200000 400000 600000 800000 1000000 2000000)
FILE_SIZES=(1024 2048 3072 4096 5120 102400)

echo 'Running all tests...'
for i in {0..5}
do
   echo "test number $i..."
    for j in {1..5}
    do
        echo "FILE SIZE: ${FILE_SIZES[$i]}"
        echo "NUMBER OF PAGES: ${NUM_OF_PAGES[$i]}"
        # echo "$(pwd)"
        ./qosdb file${FILE_SIZES[$i]} ${NUM_OF_PAGES[$i]}
        echo ""
        echo "-----------------"
        echo ""
    done
    rm file*
done

