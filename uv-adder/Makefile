UNAME=$(shell uname)

LIBUV=./deps/libuv/uv.a
CFLAGS=

all: sv cl

sv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv 

cl: cl.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include cl.c $(LIBUV)   -o cl

clean:
	rm -f sv
