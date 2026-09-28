#!/bin/bash

for t in $(seq 100 107); do
  ./notec_test_1 "$t"
done
for t in $(seq 200 201); do
  ./notec_test_2 "$t"
done
for t in $(seq 300 302); do
  ./notec_test_3 "$t"
done
for t in $(seq 400 401); do
  ./notec_test_16 "$t"
done
