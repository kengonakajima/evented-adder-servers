UNAME=$(shell uname)

LIBUV=./deps/libuv/uv.a
CFLAGS=

all: $(UNAME)

Linux: linsv lincl
Darwin: sv cl

sv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv 

cl: cl.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include cl.c $(LIBUV)   -o cl


linsv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv  -lpthread -lrt -lm

lincl: cl.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include cl.c $(LIBUV)   -o cl -lpthread -lrt -lm


clean:
	rm -f sv cl linsv lincl
