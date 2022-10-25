gflag=-g  # empty the variable in order to quickly get rid of debug info

all: bin/tests bin/example

bin/tests: obj/notec.o obj/tests.o obj/debug.o obj/err.o
	gcc ${gflag} obj/err.o obj/notec.o obj/tests.o obj/debug.o -lpthread -o bin/tests
bin/example: obj/notec.o obj/example.o
	gcc ${gflag} obj/notec.o obj/example.o -lpthread -o bin/example
obj/debug.o: src/debug.asm
	nasm ${gflag} -f elf64 -w+all -w+error -o obj/debug.o src/debug.asm
obj/err.o: src/err.h src/err.c
	gcc ${gflag} -c -Wall -Wextra -O2 -std=c11 -o obj/err.o src/err.c
obj/notec.o: src/notec.asm
	nasm ${gflag} -DN=$N -f elf64 -w+all -w+error -o obj/notec.o src/notec.asm
obj/example.o: src/example.c
	gcc ${gflag} -DN=$N -c -Wall -Wextra -O2 -std=c11 -o obj/example.o src/example.c
obj/tests.o: src/tests.c
	gcc ${gflag} -DN=$N -DT=$T -c -Wall -Wextra -O2 -std=c11 -o obj/tests.o src/tests.c
clean:
	rm -f obj/* bin/*
