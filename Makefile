TARGETS=client server test
CC=g++
CFLAGs=-03
EXTRAFLAGS= -lpqxx -pthread
all:$(TARGETS)

.Phony: all clean


client: client.cpp tinyxml2.cpp
	$(CC) $(CFLAGS) -pthread -o client client.cpp tinyxml2.cpp $(EXTRAFLAGS)

test:response.h functions.h functions.cpp main.cpp
	$(CC) $(CFLAGS) -o test response.h functions.h functions.cpp main.cpp $(EXTRAFLAGS)

server: tinyxml2.cpp server.cpp parser.cpp functions.h functions.cpp server1.h server1.cpp
	$(CC) $(CFLAGS) -pthread -o server tinyxml2.cpp server.cpp parser.cpp functions.h functions.cpp server1.h server1.cpp $(EXTRAFLAGS)

concurrency: testConcurrency.cpp
	$(CC) $(CFLAGS) -o concurrency testConcurrency.cpp $(EXTRAFLAGS)

clean:
	rm -f concurrency *~
