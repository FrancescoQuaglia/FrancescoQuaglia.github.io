compile:
	gcc -c moduleA.c
	gcc -c moduleB.c

order1: compile
	ld moduleA.o moduleB.o

order2: compile
	ld moduleB.o moduleA.o
