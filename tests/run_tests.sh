#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "run_tests.sh <build-directory-with-test-executable> <text file with test list>"
  exit 1
fi

IFS=";"
while read -r size seed result; do
  USER_RESULT="$("$1"/single_thread_test <<<"$size $seed" 2>/dev/null)"
  if ! [ "$USER_RESULT" == "$result" ]; then
    echo "Test $size;$seed FAIL"
    exit 1
  fi
done <"$2"
echo "All tests passed"
