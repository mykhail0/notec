test: test.o notec.o
	g++ notec.o test.o -lpthread -o test

notec.o: notec.asm
	nasm -DN=1 -f elf64 -w+all -w+error -o notec.o notec.asm

test.o: test.cpp
	g++ -c -Wall -Wextra -O2 -std=c++14 -o test.o test.cpp
