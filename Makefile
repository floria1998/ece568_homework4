CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq

all: test

test: main.cpp functions.h functions.cpp
	$(CC) $(CFLAGS) -o test main.cpp functions.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
