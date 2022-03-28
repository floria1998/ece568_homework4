TARGETS=client server test
EXTRAFLAGS = -lpqxx -lpq
CC = g++
CFLAGS = -O3
all: $(TARGETS)

clean:
	rm -f $(TARGETS)

client: client.cpp
	g++ -g -o $@ $<

test:response.h functions.h functions.cpp main.cpp
	$(CC) $(CFLAGS) -o test response.h functions.h functions.cpp main.cpp $(EXTRAFLAGS) 

server: tinyxml2.cpp server.cpp parser.cpp functions.h functions.cpp
	$(CC) $(CFLAGS) -o server tinyxml2.cpp server.cpp parser.cpp functions.h functions.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
