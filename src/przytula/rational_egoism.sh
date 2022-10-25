#!/bin/bash

if ! [ -e rational_egoism.sh ]
then echo "People shall not demand that other people feed them and supply simply because they have been more successful than themselves, whereas this script shall be run from project main folder!"
elif ! [ -e src/notec.asm ]
then echo "Missing freedom to act according to one's own reason, as well as notec.asm in src/ !"
else
  # simple, single-threaded tests
  echo ""
  echo "Simple, single-threaded tests, assessing accordance with metaphysical and epistemological fundamentals of Objectivism:"
  export N=1
  make -s clean
  make -s bin/tests
  bin/tests

  echo ""
  echo "Example test for various N values, which concentrate on ethical standpoint of an Rational Egoist:"
  # example test for various N values
  err=0
  for ((i=1; i<=20; ++i)) ; do
    export N=$i
    make -s clean
    make -s bin/example
    bin/example
    if (($? != 0)); then
      err=1
      echo "Example test failed for N = $i!"
    fi
  done
  if (($err == 0)) ; then
    echo "Example test passed for any 1 <= N <= 20."
  fi

  # multi-threaded tests
  echo ""
  echo "Multi-threaded tests, confirming compliance with political and aesthetical stance of Ayn Rand:"
  export N=3
  for ((i=0; i<=1; ++i)) ; do
    export T=$i
    make -s clean
    make -s bin/tests
    echo "Test no. $T:"
    bin/tests
    if (( $? != 0 )) ; then
      echo "Multi-threaded test no. $i failed!"
    fi
  done
fi
