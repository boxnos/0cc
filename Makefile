SHELL=/bin/bash

default : 0cc
#	bash -x test.sh
	./test.sh

0cc : 0cc.c

clean :
	rm -f tmp tmp.s 0cc
