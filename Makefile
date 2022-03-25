TARGETS=client server

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

client: client.cpp
	g++ -g -o $@ $<


server: tinyxml2.cpp server.cpp parser.cpp
	g++ -g -o server tinyxml2.cpp server.cpp parser.cpp


