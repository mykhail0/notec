#!/bin/bash

if ! [ -e test.sh ]; then
  echo "This script shall be run from project main folder!"
elif ! [ -e src/notec.asm ]; then
  echo "Missing notec.asm in src/ !"
else
  # simple, single-threaded tests
  echo ""
  echo "Simple, single-threaded tests:"
  export N=1
  make -s clean
  make -s bin/tests
  bin/tests

  echo ""

  # multi-threaded tests
  echo ""
  echo "Multi-threaded tests:"
  export N=3
  for ((i = 0; i <= 1; ++i)); do
    export T=$i
    make -s clean
    make -s bin/tests
    echo "Test no. $T:"
    if ! bin/tests; then
      echo "Multi-threaded test no. $i failed!"
    fi
  done
fi
