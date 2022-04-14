#include "functions.h"
#include "parser.h"
#include "server1.h"
#include "thread_pool/threadpool.h"
#include <cstring>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <thread>
#include "server1.h"
#include <vector>

mutex data;
void sendBack(string m1,int client_connection_fd,connection *C,database dataBase);
using namespace std;
using namespace tinyxml2;
vector<string> parseXml(int total,string totalRequest)
{
  vector<string> res;
  int x = 0;
  while (x<total)
  {
    int y = totalRequest.find("\n",x);    
    string number = totalRequest.substr(x,y);
    //  cout<<number<<endl;
    string xml = totalRequest.substr(y,stoi(number)+2);
    res.push_back(xml);
    x = y;
    x = x+stoi(number)+2;
  }
  return res;
}
  

void responseClient(int client_connection_fd)
{
  connection *C;
  database dataBase;
  dataBase.openDatabase(&C);
  // receiveInfo(client_connection_fd,C);
    int total = 0;
   // vector<string> ans;
   int res_length = 0;
   string totalRequest="";
    while(1)
    {
      char msg[65536]={0};
      res_length = recv(client_connection_fd,msg,sizeof(msg),0);
      
      if (res_length<=0)
      {
	 break;
      }

    
     string msg_str(msg,res_length);
     sendBack(msg_str,client_connection_fd,C,dataBase);
     totalRequest.append(msg_str);
     total+=res_length;   
   }
    //  cout<<"a"<<endl;
    // ans = parseXml(total,totalRequest);
    // cout<<"b"<<endl;
    // return ans;
  /* for (int i = 0;i<ans.size();i++)
    {
      cout<<ans[i]<<endl;
      }*/
  // cout<<"c"<<endl;
  // int res_length = 0;
  // string totalRequest="";
  // cout<<ans.size()<<endl;
  //for (int i = 0;i<ans.size();i++)
  // {
    //      cout<<i<<endl;
}

void sendBack(string m1,int client_connection_fd,connection * C,database dataBase) 
{
    parser p;
// string m1 = ans[i];
    string x = m1.substr(m1.find("\n") + 1);
    const char *m = x.c_str();
    string top = p.parsexmlTop(m); 
    string res;
    if (top == "create") {
      XMLDocument *docNew = new XMLDocument();
      XMLDeclaration *declaration = docNew->NewDeclaration();
      docNew->InsertFirstChild(declaration);
      XMLElement *root = docNew->NewElement("results");
      docNew->InsertEndChild(root);     
      vector<account> newAccount = p.createAccount(m);     
      
      for (int i = 0; i < newAccount.size(); i++) {
        if (newAccount[i].account1 == 1) {  
	  data.lock();
	  int suc = dataBase.createAccount(stod(newAccount[i].balance), newAccount[i].account_id, C);
	  data.unlock();
	  
          if (suc == 0) {
	    
            XMLElement *type = docNew->NewElement("error");
            type->SetText("Account already exists");
            string one = newAccount[i].account_id;
            type->SetAttribute("id", one.c_str());
            root->InsertEndChild(type);
          } else {
            XMLElement *type = docNew->NewElement("created");
            string one = newAccount[i].account_id;
            type->SetAttribute("id", one.c_str());
            root->InsertEndChild(type);
          }

        } else if (newAccount[i].position == 1) {
          int m1 = stoi(newAccount[i].shares);

	   data.lock();
	  int suc = dataBase.createPosition(newAccount[i].account_id,newAccount[i].symbol, m1, C);
	   data.unlock();
          if (suc == -1) {
            XMLElement *type = docNew->NewElement("error");
            type->SetText("Account does not exists");
            string one = newAccount[i].account_id;
            string two = newAccount[i].symbol;
            type->SetAttribute("id", one.c_str());
            type->SetAttribute("sym", two.c_str());
            root->InsertEndChild(type);
          } else {
            XMLElement *type = docNew->NewElement("created");
            string one = newAccount[i].account_id;
            string two = newAccount[i].symbol;
            type->SetAttribute("id", one.c_str());
            type->SetAttribute("sym", two.c_str());
            root->InsertEndChild(type);
          }
        }
      }
      XMLPrinter printer;
      docNew->Print(&printer);
      res = printer.CStr();
      
    } else if (top == "transactions") {
      XMLDocument *doc = new XMLDocument();
      doc->Parse(m);
      XMLElement *rootElement = doc->RootElement(); 
      string account_id = rootElement->Attribute("id");
      XMLDocument *docNew = new XMLDocument();
      XMLDeclaration *declaration = docNew->NewDeclaration();
      docNew->InsertFirstChild(declaration);
      XMLElement *root = docNew->NewElement("results");
      docNew->InsertEndChild(root);
      for (XMLElement *child = rootElement->FirstChildElement(); child;
        child = child->NextSiblingElement()) {
        string n = child->Value();
        XMLElement *ordr;
        if (n == "order") {
          string symbol = child->Attribute("sym");
          string amount = child->Attribute("amount");
          string limit = child->Attribute("limit");

          int type;
          amount[0] == '-' ? type = 2 : type = 1;
          int transId = 0;
	  data.lock();
          transId = dataBase.createOpen(account_id, stod(limit), stoi(amount),symbol, type, C);
	  data.unlock();
	  
          if (transId == -2) {
            ordr = docNew->NewElement("error");
            ordr->SetAttribute("sym", child->Attribute("sym"));
            ordr->SetAttribute("amount", child->Attribute("amount"));
            ordr->SetAttribute("limit", child->Attribute("limit"));
            ordr->SetText("There is no such account!");
            root->InsertEndChild(ordr);
          } else if (transId == -1) {
            ordr = docNew->NewElement("error");
            ordr->SetAttribute("sym", child->Attribute("sym"));
            ordr->SetAttribute("amount", child->Attribute("amount"));
            ordr->SetAttribute("limit", child->Attribute("limit"));
            ordr->SetText("Invalid balance number or share number");
            root->InsertEndChild(ordr);

          } else {
            ordr = docNew->NewElement("opened");
            string id1 = to_string(transId);
	    const char *tran_id = id1.c_str();
            ordr->SetAttribute("sym", child->Attribute("sym"));
            ordr->SetAttribute("amount", child->Attribute("amount"));
            ordr->SetAttribute("limit", child->Attribute("limit"));
            ordr->SetAttribute("id",transId );
            root->InsertEndChild(ordr);
          }

        } else if (n == "query") {
          string open_id = child->Attribute("id");

	  data.lock();
          vector<response> res = dataBase.queryDB(account_id, open_id, C);
	  data.unlock();
	  
          ordr = docNew->NewElement("status");
	  ordr->SetAttribute("id",open_id.c_str());
	  if (res.size()==0)
	    {
	      XMLElement *child1 = docNew->NewElement("error");
	      child1->SetText("Invalid trans Id");
	      ordr->InsertEndChild(child1);
	   }
	  else
	   {
	    for (int i = 0; i < res.size(); i++) {
            XMLElement *child1;
            if (res[i].cancel == 1) {
              child1 = docNew->NewElement("canceled");
              const char *shares = res[i].shares_c.c_str();
              child1->SetAttribute("shares", shares);
              const char *time = res[i].time_c.c_str();
              child1->SetAttribute("time", time);
            } else if (res[i].executed == 1) {
              child1 = docNew->NewElement("executed");
              const char *shares = res[i].shares_e.c_str();
              child1->SetAttribute("shares", shares);
              const char *time = res[i].time_e.c_str();
              child1->SetAttribute("time", time);
              const char *price = res[i].price_e.c_str();
              child1->SetAttribute("time", price);
            } else if (res[i].open == 1) {
              child1 = docNew->NewElement("open");
              const char *shares = res[i].shares_o.c_str();
              child1->SetAttribute("shares", shares);
            }
	    
            ordr->InsertEndChild(child1);
	    }
          }
          root->InsertEndChild(ordr);
        } else if (n == "cancel") {
          string open_id = child->Attribute("id");

	  data.lock();
          vector<response> res = dataBase.cancel(account_id, open_id, C);
	  data.unlock();
	  
          ordr = docNew->NewElement("status");
	  if (res.size() == 0)
	    {
	      XMLElement * child1=docNew->NewElement("error");
	      child1->SetText("Invalid trans Id");
	      ordr->InsertEndChild(child1);
            }
	  else
	    {
          for (int i = 0; i < res.size(); i++) {
            XMLElement *child1;
            if (res[i].cancel == 1) {
              child1 = docNew->NewElement("canceled");
              const char *shares = res[i].shares_c.c_str();
              child1->SetAttribute("shares", shares);
              const char *time = res[i].time_c.c_str();
              child1->SetAttribute("time", time);
            } else if (res[i].executed == 1) {
              child1 = docNew->NewElement("executed");
              const char *shares = res[i].shares_e.c_str();
              child1->SetAttribute("shares", shares);
              const char *time = res[i].time_e.c_str();
              child1->SetAttribute("time", time);
              const char *price = res[i].price_e.c_str();
              child1->SetAttribute("time", price);
            }
            ordr->InsertEndChild(child1);
          }
	    }
          root->InsertEndChild(ordr);
        } else {
          ordr = docNew->NewElement("error");
          ordr->SetText("Invalid tag");
        }
      }
      XMLPrinter printer;
      docNew->Print(&printer);
      res = printer.CStr();
      } else {
      cerr << "top level" << endl;
     }
    
     const char *mess = res.c_str();
     C->disconnect();
     cout<<mess<<endl;
     send(client_connection_fd, mess, strlen(mess), 0);
    }
 

int main(int argc, char *argv[]) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port = "12345";

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  }

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
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
   C->disconnect();
   while (1) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      cerr << "Error: cannot accept connection on socket" << endl;
      return -1;
    }   
    try{
     thread t1(&responseClient,client_connection_fd);
     t1.detach();
    }catch(exception & e)
      {
	cout<<e.what() <<endl;
      }
   }
  freeaddrinfo(host_info_list);
  close(socket_fd);
  return 0;
}
