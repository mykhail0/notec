#!/bin/bash

run_test() {
  echo -n "test $1 $2 with time limit $3 s: "
  timeout "${3}s" "./notec_test_$1" "$2"
  retval=$?
  if [ $retval -ne 0 ]; then
    echo "FAIL: $retval"
    exit 1
  fi
  echo "OK"
}

for t in $(seq 100 107); do
  run_test 1 "$t" 1
done
for t in $(seq 200 201); do
  run_test 2 "$t" 1
done
for t in $(seq 300 302); do
  run_test 3 "$t" 1
done
for t in $(seq 400 401); do
  run_test 16 "$t" 10
done
