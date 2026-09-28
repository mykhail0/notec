This directory includes several tests, some created by students (abi-test,
unit-tests, W-test, single_thread_test), some released after the assignment was
graded (run_test.cpp). README_single_thread_test.txt describes
single_thread_test. The rest of this file describes run_test.cpp (used to grade
the assignment).

15 tests checking the correctness of notec function:
100–107, 200, 201, 300–302, 400, 401.
Tests 100–107 are single threaded.
Tests 200 and 201 work with 2 threads.
Tests 300–302 work with 3 threads.
Tests 400 and 401 work with 16 threads.

All tests above check conforming to ABI, but only after the test result was
correct. Stack should be properly aligned, registers used correctly.

Tests can have the following results:
– pass
– ABI fail
– wrong result
– crash
– time limit exceeded
– serious crash
– does not compile or link

Code section + data section of a compiled program with N = 1 should be <= 512
bytes for full marks.
