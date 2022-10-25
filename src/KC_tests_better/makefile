PROGRAMS = example abi-test unit-tests W-test
CC = gcc
N = 10
CFLAGS = -DN=$(N) -Wall -Wextra -O2 -std=c11

all: $(PROGRAMS)

notec.o: notec.asm
	nasm -DN=$(N) -f elf64 -o $@ $<

example.o: example.c
	$(CC) -c $(CFLAGS) -o $@ $<

example: example.o notec.o
	$(CC) -pthread -o $@ $^

unit-tests.o: unit-tests.c
	$(CC) -c $(CFLAGS) -o $@ $<

unit-tests: unit-tests.o notec.o
	$(CC) -pthread -o $@ $^

abi-test.o: abi-test.asm
	nasm -DN=$(N) -f elf64 -o $@ $<

abi-test: abi-test.o notec.o
	ld  -o $@ $^

W-test.o: W-test.c
	$(CC) -c $(CFLAGS) -o $@ $<

W-test: W-test.o notec.o
	$(CC) -pthread -o $@ $^

.PHONY: all clean

clean:
	rm -rf $(PROGRAMS) *.o
