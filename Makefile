SHELL=/bin/bash

default : 0cc vector_test
#	bash -x test.sh
	./vector_test
	./test.sh

0cc : 0cc.c

vector_test : vector_test.o vector.o
	gcc -o $@ $^

clean :
	rm -f tmp tmp.s 0cc *.o
