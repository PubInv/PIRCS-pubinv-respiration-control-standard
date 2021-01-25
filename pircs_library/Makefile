all: test_PIRCS


# These shenannigans with the extension are necessary to prevent
# platformio, which automatically makes .c files, from creating
# unneeded symbol references, which in particular cause a problem
# if compiling on 8 and 32 bit architectures at the same time,
# but allow you use the tests even if you are not using platform io
test_PIRCS: Makefile test_PIRCS.c.ext PIRCS.o
	cp test_PIRCS.c.ext test_PIRCS.c
	gcc -o test_PIRCS test_PIRCS.c PIRCS.o
	rm test_PIRCS.c

PIRCS.o: Makefile PIRCS.cpp PIRCS.h
	gcc -x c -c PIRCS.cpp
