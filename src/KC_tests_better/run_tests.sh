#!/bin/bash

make

echo "Example:"
./example

echo "Unit-tests:"
./unit-tests
echo "ABI-test:"
./abi-test
echo "W-test:"
./W-test
