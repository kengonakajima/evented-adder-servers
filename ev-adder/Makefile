UNAME=$(shell uname)

LIBEV=deps/libev
EVOBJS=$(LIBEV)/ev.o $(LIBEV)/event.o
CFLAGS=-g -Ideps/libev
#-O3

all: $(UNAME)

Linux: linsv lincl
Darwin: sv cl

sv: sv.c $(EVOBJS)
	gcc $(CFLAGS)  sv.c $(EVOBJS) -o sv 

cl: cl.c $(EVOBJS)
	gcc $(CFLAGS) cl.c $(EVOBJS) -o cl


linsv: sv.c $(EVOBJS)
	gcc $(CFLAGS) sv.c $(EVOBJS) -o sv  

lincl: cl.c $(EVOBJS)
	gcc $(CFLAGS) cl.c $(EVOBJS) -o cl 


clean:
	rm -f sv cl linsv lincl
