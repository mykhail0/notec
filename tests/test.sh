#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Run like: $0 build_directory"
    exit 1
fi

for f in example unit-tests abi-test W-test; do
    if ! "$1/$f"; then
        echo "$f failed"
        exit 1
    fi
done
