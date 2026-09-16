#/bin/bash

bakery:
	gcc bakery.c -o bakery -lpthread

check:
	gcc check.c -o check

all:	bakery check
	@echo 'built all executables'

clean:
	@touch ./bakery
	rm ./bakery
	@touch ./check
	rm ./check

run:
	./bakery > err 
	sort -g err > err1
	./check < err1
