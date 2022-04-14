#include "tinyxml2.h"
#include <cstring>
#include "parser.h"
#include <iostream>
#include <vector>

using namespace tinyxml2;
using namespace std;

string parser::createResponse()
{
  string buf;
  XMLDocument doc;
  XMLDeclaration * declaration = doc.NewDeclaration();
  doc.InsertFirstChild(declaration);
  XMLElement * root = doc.NewElement("Repsonse");
  doc.InsertEndChild(root);
  XMLElement * type = doc.NewElement("Type");
  type->SetText("success");
  root->InsertEndChild(type);
  XMLPrinter printer;
  doc.Print(&printer);
  buf = printer.CStr();
  return buf;
}

string parser::createSuccess(string id,string sym,int great)
{
  string buf;
  XMLDocument doc;
  XMLDeclaration * declaration = doc.NewDeclaration();
  doc.InsertFirstChild(declaration);
  XMLElement * root = doc.NewElement("results");
  doc.InsertEndChild(root);
  XMLElement * type = doc.NewElement("created");
  const char * id1 = id.c_str();
  type->SetAttribute("id",id1);
  root->InsertEndChild(type);

  XMLElement * pos;
   if (great)
   {
       pos = doc.NewElement("created");
   }
   else
   {
      pos = doc.NewElement("error");
      pos->SetText("Cannot create position");
   }

  const char * m = sym.c_str();
   
  pos->SetAttribute("sym",m);
  pos->SetAttribute("id",id1);
  
  root->InsertEndChild(pos);
  XMLPrinter printer;
  doc.Print(&printer);
  buf = printer.CStr();
  return buf;
}

string parser::createError(string id)
{
  string buf;
  XMLDocument doc;
  XMLDeclaration * declaration = doc.NewDeclaration();
  doc.InsertFirstChild(declaration);
  XMLElement * root = doc.NewElement("results");
  doc.InsertEndChild(root);
  XMLElement * type = doc.NewElement("error");
  type->SetText("Account already exists");
  const char * id1 = id.c_str();
  type->SetAttribute("id",id1);
  root->InsertEndChild(type);
  XMLPrinter printer;
  doc.Print(&printer);
  buf = printer.CStr();
  return buf;
}


string parser::parsexmlTop(const char * buffer)
{
  //cout<<buffer<<endl;
  XMLDocument * doc = new XMLDocument();
  doc->Parse(buffer);
  XMLElement * rootElement = doc->RootElement();
  // cout<<rootElement->Value()<<endl;
  const char * rootName = rootElement->Value();
  string m = rootName;
  // cout<<rootName<<endl;
  return rootName;
}

vector<account> parser::createAccount(const char *buffer)
{
  vector<account> res;
  //account m;
  XMLDocument * doc = new XMLDocument();
  doc->Parse(buffer);
  XMLElement * rootElement = doc->RootElement();

  for (XMLElement * child = rootElement->FirstChildElement();child;child=child->NextSiblingElement())
  {
    
    //    account m;
      //  m.account_id = child->Attribute("id");
      string n = child->Value();
      //  cout<<n<<endl;
       if (n =="account")
	 { account m;
	   m.account1 = 1;
	   m.account_id = child->Attribute("id");
	   m.balance = child->Attribute("balance");
	   res.push_back(m);
	}
       
       if (n == "symbol")
	{
	  string symbol = child->Attribute("sym");
	  for (XMLElement * cc = child->FirstChildElement();cc;cc = cc->NextSiblingElement())
	    {
	      // cout<< cc->Value() <<endl;
	      account m;
	      m.position = 1;
	      m.symbol = symbol;
	      // XMLElement * share1 = child->FirstChildElement("account");
	      m.account_id = cc->Attribute("id");
	      m.shares = cc->GetText();
	  // cout<<"shares:"<<m.shares<<endl;
	      res.push_back(m);
	    }
	}
       // res.push_back(m);
     }
  return res;
}

transactions parser::parseTransec(const char * buffer)
{
  transactions res;
  XMLDocument * doc = new XMLDocument();
  doc->Parse(buffer);
  XMLElement * rootElement = doc->RootElement(); //get transaction
  res.account_id = rootElement->Attribute("id");
  cout<<res.account_id<<endl;

   for (XMLElement * child = rootElement->FirstChildElement();child;child=child->NextSiblingElement())
    {
      string m = child->Value();
      cout<<m<<endl;
       if (m =="cancel")
	{
	  res.cancel = true;
	  res.cancel_id = child->Attribute("id");
	}
      else if (m == "query")
	{
	  res.query = true;
	  res.query_id = child->Attribute("id");
	}
      else if (m=="order")
	{
	  res.order = true;
	  res.symbol = child->Attribute("sym");
	  res.amount = child->Attribute("amount");
	  res.limit = child->Attribute("limit");
	}
      else
	{
	  std::cerr<<"Invalid tag"<<std::endl;
	}
    }

   // cout<<res.order<<endl;
  return res;
}
