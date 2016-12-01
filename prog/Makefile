BIN=.

#FLAGS= -g -O0 -Wall -Wno-unused-result -std=gnu99 -pedantic
FLAGS= -O3 -Wall -Wno-unused-result -std=gnu99 -pedantic
MC920LIB    = -L../lib -lmc920 -lm 
MC920FLAGS  = -I../include 

PROG_SRC    = $(basename $(shell find ./ -maxdepth 1 -name '*.c'))

all:
	make libift
	make $(PROG_SRC)

libift:
	cd ../; $(MAKE) ; cd -\

$@.c: libmc920 $@.c
	gcc $(FLAGS) $@.c -o $(BIN)/$@ $(MC920FLAGS) $(MC920LIB) ; \

clean:	
	rm -f BC mip planar_cut reformat_planar rendering to2d wireframe
