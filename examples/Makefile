
all: test_dot

INC_DIR=../include
INC_PAPI=/home/cpelisseverdoux/papi

PAPI_HOME=/home/cpelisseverdoux/papi
PAPI_INCLUDE = $(PAPI_HOME)/include
PAPI_LIBRARY = -L$(PAPI_HOME)/lib -lpapi -Wl,-rpath,$(PAPI_HOME)/lib


LIB_DIR=../lib

LIBST=-lmnblas
LIBDYN=-lmnblasdyn

OPTIONS_COMPIL  = -g -Wall -O2 -fPIC -I$(INC_DIR) -lm

OPTIONS_LINK_STATIC  =   -fopenmp -L$(LIB_DIR) -L$(INC_PAPI)/lib  $(LIBST)
OPTIONS_LINK_DYN  = -fopenmp -L$(LIB_DIR) $(LIBDYN)

test_dot: test_dot.o flop.o $(LIB_DIR)/libmnblas.a
	gcc -o test_dot test_dot.o flop.o $(OPTIONS_LINK_STATIC) $(PAPI_LIBRARY)

flop.o: flop.c flop.h
	gcc $(OPTIONS_COMPIL) -c flop.c

test_dot.o: test_dot.c $(INC_DIR)/mnblas.h
	gcc $(OPTIONS_COMPIL) -I$(PAPI_INCLUDE) -c test_dot.c 

clean:
	rm -f *.o test_dot
#gcc -I$HOME/papi-install/include -L$HOME/papi-install/lib papi_example.c -lpapi -o papi_example
