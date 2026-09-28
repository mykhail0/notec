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

Results of the tests are encoded as a string of 15 characters with the following
meaning:
  P – pass
  A – ABI fail
  R – wrong result
  C – crash
  T – time limit exceeded
  H – serious crash
  M – does not compile or link

Time limits for tests 100-302 are 1 s, for tests 400 and 401 it's 10 s.

Code section + data section of a compiled program with N = 1 should be <= 512
bytes for full marks.

Ocena jest ograniczana do przedziału od 0 do 5 punktów i zaokrąglana w dół
z dokładnością do 0,1 punktu.

To run tests, first compile:

make NNNN=16

Later in the directory with the solution, run:

make -f tests_directory/test_makefile binary=notec script_dir=tests_directory source=notec.asm NNNN=16

where test_dir is the test directory.

Order to run tests:

./notec_test_1 100
./notec_test_1 101
./notec_test_1 102
./notec_test_1 103
./notec_test_1 104
./notec_test_1 105
./notec_test_1 106
./notec_test_1 107
./notec_test_2 200
./notec_test_2 201
./notec_test_3 300
./notec_test_3 301
./notec_test_3 302
./notec_test_16 400
./notec_test_16 401
