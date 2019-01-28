SHELL=/bin/bash
CFLAGS=-Wall

default : 0cc vector_test map_test
	./vector_test
	./map_test
	./test.sh

0cc : 0cc.o map.o vector.o token.o utility.o

vector_test : vector_test.o vector.o expect.o

map_test : map_test.o vector.o map.o expect.o

clean :
	rm -f 0cc *_test *.o tmp tmp.s
