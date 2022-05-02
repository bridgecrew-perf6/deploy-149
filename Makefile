all: main main_API

CC=gcc
CFLAGS=-O2 -pipe -Wall
LDFLAGS=-O2 -L/usr/local/include -lqt
LIBFLAGS=-lcurl -ljson-c


main: main.o
	$(CC) -o main $(LDLAGS) main.o

main_API: main_API.o
	$(CC) -o main_API $(LDLAGS) main_API.o $(LIBFLAGS)

main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c

main_API.o: main_API.c
	$(CC) $(CFLAGS) -c main_API.c

clean :
	rm main_API main