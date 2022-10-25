Simple tests to ensure that the calculations are correct. The tests only run on a single thread and do not use the W operation.

Due to the test size a test generation program is included instead of the test content. To compile the generator, put your notec.asm file in the same directory as the Makefile, and run `make`.

After running `make`, the tests can be ran by running `./run_tests.sh <path to the text file with tests>`, such as `./run_tests.sh tests_tiny.txt`.

It is expected that running the tests with tests_medium.txt and tests_big.txt files might take a few minutes. My solution takes around 8 minutes to execute these tests on my machine.

The `run_tests.sh` immediately exits on a test in which the output did not match with the expected one. In such a case, you will see a line such as `Test 8;788: FAIL`. In this case, to see the test that failed run the `./test` executable and type `8 788` (instead of the ; separator use a space). The test and execution time will be printed to the console using standard error output.
