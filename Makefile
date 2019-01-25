SHELL=/bin/bash

default : 0cc vector_test map_test
#	bash -x test.sh
	./vector_test
	./map_test
	./test.sh

0cc : 0cc.o map.o vector.o

vector_test : vector_test.o vector.o expect.o

map_test : map_test.o vector.o map.o expect.o

clean :
	rm -f tmp tmp.s 0cc *.o
