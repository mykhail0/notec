run_tests.sh, *.txt and single_thread_test.cpp description.

Simple tests to ensure that the calculations are correct. The tests only run on
a single thread and do not use the W operation.

Due to the test size a test generation program is included instead of the test
content.

It is expected that running the tests with tests_medium.txt and tests_big.txt
files might take a few minutes.

The `run_tests.sh` immediately exits on a test in which the output did not match
with the expected one. In such a case, you will see a line such as
`Test 8;788: FAIL`. In this case, to see the test that failed run the `./test`
executable and type `8 788` (instead of the ; separator use a space). The test
and execution time will be printed to the console using standard error output.
