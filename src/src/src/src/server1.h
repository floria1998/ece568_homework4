#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

using namespace std;
int get_sock();
int accept_sock(int socket_fd,string *ip);
