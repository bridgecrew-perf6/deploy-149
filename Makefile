all: main_API

CC=gcc
CFLAGS=-O2 -pipe -Wall
LDFLAGS=-O2 -L/usr/local/include -lqt
LIBFLAGS=-lcurl -lcjson -lpapi
#LIBFLAGS=-lcurl -lcjson -L./papi-perf_event/lib/libpapi.a
SRC_DIR=./src
EXA_DIR=./examples


main_API: main_API.o
	make -C $(SRC_DIR)
	make -C $(EXA_DIR)
	$(CC) -o main_API $(LDLAGS) main_API.o $(LIBFLAGS)

main_API.o: main_API.c
	$(CC) $(CFLAGS) -c main_API.c

clean :
	rm main_API *.o
	make -C $(SRC_DIR) clean
	make -C $(EXA_DIR) clean
