#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "run_tests.sh <text file with test list>"
    exit 1
fi

IFS=";"
while read size seed result
 do
   echo -n "Test $size;$seed: "
   USER_RESULT="$(./test <<< "$size $seed" 2>/dev/null)"
   if [ "$USER_RESULT" == "$result" ]; then
     echo "OK"
   else
     echo "FAIL"
     exit 1
   fi
 done < $1
echo "All tests passed"