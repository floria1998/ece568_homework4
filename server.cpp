#include "functions.h"
#include "parser.h"
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include "server1.h"

using namespace std;
using namespace tinyxml2;

int main(int argc, char *argv[]) {
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
  int socket_fd = get_sock();
  // cout << "Waiting for connection on port " << port << endl;

   
   string ip;
   int client_connection_fd = accept_sock(socket_fd,&ip); 
    
   char buffer[65535];

   recv(client_connection_fd, buffer, 65535,0); // receive information from client
   buffer[65535] = 0;

   parser p;
   string m1 = buffer;
   string x = m1.substr(m1.find("\n") + 1);
   const char *m = x.c_str();

   string top = p.parsexmlTop(m); // parse the xml

  // start sql  
   string res;
   if (top == "create")
   {
     XMLDocument * docNew = new XMLDocument();
     XMLDeclaration * declaration = docNew->NewDeclaration();
     docNew->InsertFirstChild(declaration);
     XMLElement * root = docNew->NewElement("results");
     docNew->InsertEndChild(root);
     vector<account> newAccount = p.createAccount(m);
      for (int i = 0;i<newAccount.size();i++)
     { 
      
       if (newAccount[i].account1==1)
	 {
	   if (dataBase.createAccount(stod(newAccount[i].balance), newAccount[i].account_id, C)==0)
	     {
	         XMLElement * type = docNew->NewElement("error");
                 type->SetText("Account already exists");
		 string one = newAccount[i].account_id;
		 type->SetAttribute("id",one.c_str());
		 root->InsertEndChild(type);
	     }
	   else
	     {
	        XMLElement * type = docNew->NewElement("created");
                string one = newAccount[i].account_id;
		type->SetAttribute("id",one.c_str());
		root->InsertEndChild(type);
	      }
       
	 }
       else if (newAccount[i].position==1)
	 {
	   
	   int m1 = stoi(newAccount[i].shares);
	   if (dataBase.createPosition(newAccount[i].account_id,newAccount[i].symbol,m1,C)==0)
	    {
	         XMLElement * type = docNew->NewElement("error");
                 type->SetText("Account already exists");
		 string one = newAccount[i].account_id;
		 string two = newAccount[i].symbol;
		 type->SetAttribute("id",one.c_str());
		 type->SetAttribute("sym",two.c_str());	 
		 root->InsertEndChild(type);
	    }
	   else
	    {
	        XMLElement * type = docNew->NewElement("created");
                string one = newAccount[i].account_id;
		string two = newAccount[i].symbol;
		type->SetAttribute("id",one.c_str());
		type->SetAttribute("sym",two.c_str());
		root->InsertEndChild(type);
	     }     

	 }
     }
       XMLPrinter printer;
       docNew->Print(&printer);
       res = printer.CStr();   
  }
  else if (top == "transactions")
  {
    dataBase.createAccount(200,"1234",C);
       XMLDocument * doc = new XMLDocument();
       doc->Parse(m);
       XMLElement * rootElement = doc->RootElement(); //get transaction
       string account_id = rootElement->Attribute("id");
       XMLDocument * docNew = new XMLDocument() ;
       XMLDeclaration * declaration = docNew->NewDeclaration();
       docNew->InsertFirstChild(declaration);
       XMLElement * root = docNew->NewElement("results");
       docNew->InsertEndChild(root);

       for (XMLElement * child = rootElement->FirstChildElement();child;child=child->NextSiblingElement())
       {
       string n = child->Value();      
       XMLElement * ordr;
       if (n == "order")
	 {
	   string symbol = child->Attribute("sym");
	   string amount = child->Attribute("amount");	   
	   string limit = child->Attribute("limit");
	   int nn = stoi(amount); 
	   int type;
	   // amount[0]=='-'?type=2:type=1;
	   if (nn<0)
	     {
	       type = 2;
	     }
	   else
	     {
	       type = 1;
	     }
	   // int transId =0;
	     string transId = dataBase.createOpen(account_id,stod(limit),stoi(amount),symbol,type,C);
	     if (transId == "-1")
	     {
	         ordr = docNew->NewElement("error");
		 ordr->SetAttribute("sym",child->Attribute("sym"));
		 ordr->SetAttribute("amount",child->Attribute("amount"));
		 ordr->SetAttribute("limit",child->Attribute("limit"));
		 ordr->SetText("Invalid balance number or share number or account number");
	         root->InsertEndChild(ordr);
	      
	     }
	    else
	    {
	        ordr = docNew->NewElement("open");
		string id1 = to_string(transId);
		const char * tran_id = id1.c_str();
		ordr->SetAttribute("sym",child->Attribute("sym"));
		ordr->SetAttribute("amount",child->Attribute("amount"));
		ordr->SetAttribute("limit",child->Attribute("limit"));
		ordr->SetAttribute("id",tran_id);
		root->InsertEndChild(ordr);
	    }
	       
	 }    
       else if (n == "query")
	 {
	   string open_id = child->Attribute("id");
	   vector<response> res =dataBase.queryDB(account_id,open_id,C);
	   ordr = docNew->NewElement("status");
	    for (int i = 0;i<res.size();i++)
	     {
	       XMLElement * child1; 
	       if (res[i].cancel==1)
		 {
		   child1 = docNew->NewElement("canceled"); 
		   const char * shares = res[i].shares_c.c_str();  
		   child1->SetAttribute("shares",shares);
		   const char * time = res[i].time_c.c_str();  
		   child1->SetAttribute("time",time);
		 }
	        else if (res[i].executed==1)
		 {
		   child1 = docNew->NewElement("executed"); 
		   const char * shares = res[i].shares_e.c_str();  
		   child1->SetAttribute("shares",shares);
		   const char * time = res[i].time_e.c_str();  
		   child1->SetAttribute("time",time);
		   const char * price = res[i].price_e.c_str();  
		   child1->SetAttribute("time",price);		   
		 }
		 else  if (res[i].open==1)
		 {
		   child1 = docNew->NewElement("open"); 
		   const char * shares = res[i].shares_o.c_str();  
		   child1->SetAttribute("shares",shares);
		   
		 }

	       ordr->InsertEndChild(child1); 
	     }
	      root->InsertEndChild(ordr);
	  }
       else if (n == "cancel")
	 {
	   string open_id = child->Attribute("id");
	   vector<response> res =dataBase.cancel(account_id,open_id,C);
	   ordr = docNew->NewElement("status");
	    for (int i = 0;i<res.size();i++)
	     {
	       XMLElement * child1; 
	       if (res[i].cancel==1)
		 {
		   child1 = docNew->NewElement("canceled"); 
		   const char * shares = res[i].shares_c.c_str();  
		   child1->SetAttribute("shares",shares);
		   const char * time = res[i].time_c.c_str();  
		   child1->SetAttribute("time",time);
		 }
	        else if (res[i].executed==1)
		 {
		   child1 = docNew->NewElement("executed"); 
		   const char * shares = res[i].shares_e.c_str();  
		   child1->SetAttribute("shares",shares);
		   const char * time = res[i].time_e.c_str();  
		   child1->SetAttribute("time",time);
		   const char * price = res[i].price_e.c_str();  
		   child1->SetAttribute("time",price);		   
		 }
		 ordr->InsertEndChild(child1); 
	     }
	   root->InsertEndChild(ordr);
	 }
       else
	 {
	   
	   ordr = docNew->NewElement("error");
	   ordr->SetText("Invalid tag");
	   root->InsertEndChild(ordr);
	 }
       }
       
         XMLPrinter printer;
        docNew->Print(&printer);
        res = printer.CStr(); 
    }
      else {
    cerr << "top level" << endl;
  }

   const char *mess = res.c_str();
   send(client_connection_fd, mess, strlen(mess), 0);
    close(socket_fd);

  return 0;
}
