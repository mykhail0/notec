AS = nasm
ASFLAGS = -DN=$(N) -f elf64 -w+all -w+error
CC = gcc
CFLAGS = -DN=$(N) -Wall -Wextra -O2 -std=c11
CXX = g++
# Make empty if unneeded.
DEBUG_FLAG = -g

BUILDDIR = build
SRCDIR = src
NOTEC_SRC = $(SRCDIR)/notec.asm
TESTDIR = tests
TESTS = $(EXAMPLE) $(BUILDDIR)/abi-test $(BUILDDIR)/unit-tests \
		$(BUILDDIR)/W-test $(BUILDDIR)/single_thread_test
GRADER_OBJS = $(TESTDIR)/binary_1.o $(TESTDIR)/binary_2.o \
			  $(TESTDIR)/binary_3.o $(TESTDIR)/binary_16.o

.PHONY: all clean test grading_test

EXAMPLE = $(BUILDDIR)/example

all: $(EXAMPLE)

$(BUILDDIR)/notec.o: $(NOTEC_SRC)
	mkdir -p $(BUILDDIR)
	$(AS) $(DEBUG_FLAG) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/example.o: $(SRCDIR)/example.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(EXAMPLE): $(BUILDDIR)/notec.o $(BUILDDIR)/example.o
	$(CC) $^ -lpthread -o $@

$(BUILDDIR)/abi-test.o: $(TESTDIR)/abi-test.asm
	mkdir -p $(BUILDDIR)
	$(AS) $(DEBUG_FLAG) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/abi-test: $(BUILDDIR)/notec.o $(BUILDDIR)/abi-test.o
	$(LD) -o $@ $^

$(BUILDDIR)/unit-tests.o: $(TESTDIR)/unit-tests.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILDDIR)/unit-tests: $(BUILDDIR)/notec.o $(BUILDDIR)/unit-tests.o
	$(CC) $^ -lpthread -o $@

# W-test works only with N <= 10
$(BUILDDIR)/W-test-notec.o: $(NOTEC_SRC)
	mkdir -p $(BUILDDIR)
	$(AS) $(DEBUG_FLAG) -DN=8 -f elf64 -w+all -w+error -o $@ $<

$(BUILDDIR)/W-test.o: $(TESTDIR)/W-test.c
	mkdir -p $(BUILDDIR)
	$(CC) -c -DN=8 -Wall -Wextra -O2 -std=c11 -o $@ $<

$(BUILDDIR)/W-test: $(BUILDDIR)/W-test-notec.o $(BUILDDIR)/W-test.o
	$(CC) $^ -lpthread -o $@

$(BUILDDIR)/single_thread_test: $(BUILDDIR)/single_thread_test.o $(BUILDDIR)/single-notec.o
	$(CXX) $^ -lpthread -o $@

$(BUILDDIR)/single-notec.o: $(NOTEC_SRC)
	mkdir -p $(BUILDDIR)
	$(AS) $(DEBUG_FLAG) -DN=1 -f elf64 -w+all -w+error -o $@ $<

$(BUILDDIR)/single_thread_test.o: $(TESTDIR)/single_thread_test.cpp
	$(CXX) -c -Wall -Wextra -O2 -std=c++14 -o $@ $<

clean:
	rm -rf $(BUILDDIR)
	rm -f notec_*
	rm -f $(GRADER_OBJS)

$(GRADER_OBJS) &: $(TESTDIR)/notec_test_stub.asm $(TESTDIR)/run_test.cpp \
 $(TESTDIR)/test.hpp $(TESTDIR)/test.cpp $(TESTDIR)/test_object.hpp \
 $(TESTDIR)/notec_test_runner.hpp $(TESTDIR)/notec_test_runner.cpp
	cd $(TESTDIR) && make NNNN=16 && cd ..

grading_test: test.sh $(NOTEC_SRC) $(GRADER_OBJS)
	make -f $(TESTDIR)/test_makefile binary=notec script_dir=$(TESTDIR) source=$(NOTEC_SRC) NNNN=16
	./test.sh

test: $(EXE) $(TESTDIR) $(TESTS) grading_test
	$(TESTDIR)/test.sh build
	# $(TESTDIR)/run_tests.sh build $(TESTDIR)/tests_tiny.txt
	# $(TESTDIR)/run_tests.sh build $(TESTDIR)/tests_small.txt
	# $(TESTDIR)/run_tests.sh build $(TESTDIR)/tests_medium.txt
	# $(TESTDIR)/run_tests.sh build $(TESTDIR)/tests_big.txt
