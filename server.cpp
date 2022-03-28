#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "parser.h"
#include "functions.h"

using namespace std;
using namespace tinyxml2;


int main(int argc, char *argv[])
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port     = "12345";

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

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

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } 

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } 

  cout << "Waiting for connection on port " << port << endl;
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if

  char buffer[65535];
  
  recv(client_connection_fd, buffer, 65535, 0);// receive information from client
  buffer[65535] = 0;
   
  parser p;
  string m1 = buffer;
  string x = m1.substr(m1.find("\n")+1);
  const  char * m = x.c_str();
   
  string top =p.parsexmlTop(m); // parse the xml

  //start sql
  connection *C;
  database dataBase;
  dataBase.openDatabase(&C);
  dataBase.dropTable(C, "USER_TB");
  dataBase.dropTable(C, "POSITION_TB");
  dataBase.dropTable(C, "TRANSACTION_TB");
  dataBase.dropTable(C, "OPEN_TB");
  dataBase.dropTable(C, "EXECUTED_TB");
  dataBase.dropTable(C, "CANCEL_TB");
  dataBase.createTable(C, "createTable.sql");

  string res;
   if (top == "create")
   {
      account newAccount = p.createAccount(m);
      double m = stod(newAccount.balance);
      if (dataBase.createAccount(m, newAccount.account_id, C)==0)
	{
	  res = p.createError(newAccount.account_id);
	  cout<<res<<endl;
	}
      else
	{
	  res = p.createSuccess(newAccount.account_id);
	  cout<<res<<endl;
      	  dataBase.createPosition(newAccount.account_id, newAccount.symbol, stoi(newAccount.shares), C);
	}
   }
  else if (top == "transactions")
    {
      transactions tran = p.parseTransec(m);
      if (tran.cancel)
	{
	  dataBase.cancel(tran.cancel_id,C);
	}
      
      /* if (tran.query)
	{
	  
	}

      if (tran.order)
	{
	  double price = stod(tran.price);
	  int amount = stoi(tran.amount);
	  int type;
	  if (amount<0)
	  {
	      type = 2;
	  }
	  else
	   {
	      type = 1;
	   }
	 int tran_id = createOpen(tran.account_id,price,amount,symbol,type,C);
	 matchOneOrder(C,tran_id);
	 }*/
    }
   else
    {
      cerr<<"top level"<<endl;
    }
  
   // string res = p.createResponse();
  const char * mess = res.c_str();
  send(client_connection_fd,mess,strlen(mess),0);
  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}