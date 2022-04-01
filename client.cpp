#include "server1.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <thread>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

string m[10] = {"abc","btc","111","230","hhh","www","qqq","ccc","nnn","mmm"};

string m1[15] = {"abc","btc","111","230","hhh","www","qqq","ccc","nnn","mmm","666","777","888","000","222"};

string create()
{
  srand((unsigned)time(NULL));
  string buf;
  XMLDocument doc;
  XMLDeclaration * declaration = doc.NewDeclaration();
  doc.InsertFirstChild(declaration);
  XMLElement * root = doc.NewElement("create");
  doc.InsertEndChild(root);
  XMLElement * type = doc.NewElement("account");
  int id = rand()%100000;
  string id1 = to_string(id);
  type->SetAttribute("id",id1.c_str());
  type->SetAttribute("balance","1000");
  root->InsertEndChild(type);
  //XMLElement * root2 = doc.NewElement("symbol");
  // int which = rand()%10;
  // root2->SetAttribute("sym",m[which].c_str());
  // XMLElement * root3 = doc.NewElement("account");
  //int id2 = rand()%100000;
  // string id3 = to_string(id2);
  //  root3->SetAttribute("id",id3.c_str());
  //root3->SetText(id3.c_str());
  // root2->InsertEndChild(root3);
  // root->InsertEndChild(root2);
  XMLPrinter printer;
  doc.Print(&printer);
  buf = printer.CStr();
  string final ="";
  final = final+to_string(buf.length())+"\n"+buf;
  return final;
}

string transactions()
{
  srand((unsigned)time(NULL));
  string buf;
  XMLDocument doc;
  XMLDeclaration * declaration = doc.NewDeclaration();
  doc.InsertFirstChild(declaration);
  XMLElement * root = doc.NewElement("transactions");
  doc.InsertEndChild(root);
  XMLElement * type = doc.NewElement("order");
  int which = rand()%15;
  type->SetAttribute("sym",m1[which].c_str());
  type->SetAttribute("amount","1000");
  int price = rand()%128+1;
  
  if (price%2==0)
  {
      price = -price;
  }
  
  string price1 = to_string(price);
  type->SetAttribute("limit",price1.c_str());
  root->InsertEndChild(type);

  long trans_id = rand()%100000;
  string trans_ids = to_string(trans_id);
  XMLElement * root2 = doc.NewElement("query");
  root2->SetAttribute("trans_id",trans_ids.c_str());
  root->InsertEndChild(root2);

  XMLElement * root3 = doc.NewElement("cancel");
  trans_id+=2;
  string trans_idss = to_string(trans_id);
  root3->SetAttribute("trans_id",trans_idss.c_str());
  root->InsertEndChild(root3);
    
  XMLPrinter printer;
  doc.Print(&printer);
  buf = printer.CStr();
  string final ="";
  final = final+to_string(buf.length())+"\n"+buf;
  return final;
}

void sendReq(long i,long h)
{
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
    return;

  }
  

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return;
  }

  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;

  status =
      connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);

  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return;
  }

 for (int i = 0;i<1000;i++)
 {
  srand((unsigned)time(NULL));
  string sendString;
  long m = rand()%h;
  if (m%2 == 0)
   {
      sendString = create();
   }
  else
   {
      sendString = transactions();
   }
  
   const char * message = sendString.c_str();

     send(socket_fd, message, strlen(message)+1, 0);   
  }

 int recv1 = 1;   
 /* while (1)
   {
     char buf[65535]={0}; 
    int recv1 =  recv(socket_fd, buf, sizeof(buf), 0);
    if (recv1<=3)
      {
	break;
      }
    //	cout<<buf<<endl;
   }
 // char buf[65535]; 
  recv(socket_fd, &buf, sizeof(buf), 0);
  cout<<buf<<endl;*/
  
  
    freeaddrinfo(host_info_list);
    close(socket_fd);

  
}

int main(int argc, char *argv[])
{
  if (argc == 1)
    {
      return 0;
    }

  string second = argv[1];
  long h = stol(second);
  cout<<"begin"<<endl;
  vector<thread> threads;

  //  for (long i = 0;i<h;i++)
  // {
      
      sendReq(1,h);
      //  }
      /*
  try{
    for (long i = 0;i<h;i++)
      {
	threads[i].detach();
      }
  }
  catch(exception & e)
    {
      cout<<e.what()<<endl;
    }
  */ 
  cout<<"end"<<endl;
  return 0;
}
