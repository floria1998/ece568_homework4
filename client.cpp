#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include "server1.h"
using namespace std;

int main(int argc, char *argv[])
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char * hostname = "vcm-25297.vm.duke.edu";
  const char * port     = "12345";
  
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } 

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } 
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);

  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } 

   
       
  
   string m = "";
  ifstream input_file(argv[1]);
  if (!input_file.is_open())
  {
      cerr<<"Could not open test file"<<endl;
  }
  
  string line;
  while (getline(input_file,line))
  {
      m=m+line;
      m=m+"\n"; 
  }

  const char * message = m.c_str();
  send(socket_fd, message, strlen(message), 0); // send xml file
  //receive xml response
  char buf[65535];
  recv(socket_fd,&buf,sizeof(buf),MSG_WAITALL);

  cout<<buf<<endl;

  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}

