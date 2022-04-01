#include "server1.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  const char *hostname = "vcm-25297.vm.duke.edu";
  const char *port = "12345";

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;

  }
  //>>>>>>> f6991c68e149ae409b207e583dd95eb2d3dfd8d2

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }

  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;

  status =
      connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);

  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }

 string m = "";
  ifstream input_file(argv[1]);

  if (!input_file.is_open()) {
    cerr << "Could not open test file" << endl;
  }

  string line  = "<?xml version='1.0' encoding='UTF-8'?>\n<create>\n<account id = '123456' balance='1000' />\n<symbol sym = 'SPY'>\n<account id = '123456'>100000</account>\n</symbol>\n</create>\n";
  string x = to_string(sizeof(line));
  cout<<x<<endl;
  string final = final+x + "\n" + line;
  const char *message = final.c_str();
  cout<<message<<endl;
  send(socket_fd, message, strlen(message)+1, 0); // send xml file

  // TODO Check if not bloc
  char buf[65535];
  recv(socket_fd, &buf, sizeof(buf), 0);

  cout << buf << endl;

  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}
