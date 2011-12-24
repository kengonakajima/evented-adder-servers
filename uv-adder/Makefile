UNAME=$(shell uname)

LIBUV=./deps/libuv/uv.a
CFLAGS=

all: sv

sv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv 

linsv: sv.c $(LIBUV)
	gcc -DEV_MULTIPLICITY=1 -g -Ideps/libuv/include sv.c $(LIBUV)   -o sv -lpthread -lrt -lm


clean:
	rm -f sv
