#include "tinyxml2.h"
#include <cstring>
#include "parser.h"
#include <iostream>

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

account parser::createAccount(const char *buffer)
{
  account m;
  XMLDocument * doc = new XMLDocument();
  doc->Parse(buffer);
  XMLElement * rootElement = doc->RootElement();
  XMLElement * account1 = rootElement->FirstChildElement("account");

  std::cout<<account1->Value()<<std::endl;
  m.account_id = account1->Attribute("id");
  m.balance = account1->Attribute("balance");
  std::cout<<m.balance<<std::endl;

  XMLElement * brother = rootElement->FirstChildElement("symbol");
  m.symbol = brother->Attribute("sym");
  std::cout<<m.symbol<<std::endl;

  XMLElement * share1 = brother->FirstChildElement("account");
  m.shares = share1->GetText();
  std::cout<<m.shares<<std::endl;
 
  return m;
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

     cout<<res.order<<endl;
  return res;
}
