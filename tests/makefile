CXXFLAGS = -Wall -Wextra -O2 -std=c++17

all: binary_1.o binary_2.o binary_3.o binary_${NNNN}.o

binary_%.o: test_%.cpp.o run_test_%.cpp.o notec_test_runner_%.cpp.o notec_stub_%.asm.o
	ld --relocatable -o $@ $^

notec_stub_%.asm.o: notec_test_stub.asm
	nasm -f elf64 -DN=$* -w+all -w+error -o $@ $<

run_test_%.cpp.o: run_test.cpp test.hpp notec_test_runner.hpp test_object.hpp
	g++ -c $(CXXFLAGS) -DN=$* -o $@ $<

test_%.cpp.o: test.cpp test.hpp test_object.hpp
	g++ -c $(CXXFLAGS) -DN=$* -o $@ $<

notec_test_runner_%.cpp.o: notec_test_runner.cpp notec_test_runner.hpp test_object.hpp
	g++ -c $(CXXFLAGS) -DN=$* -o $@ $<

.PHONY: all clean

clean:
	rm -rf *.o
