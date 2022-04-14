#include <cstring>
#include "tinyxml2.h"
#include "string.h"
#include <iostream>
#include "account.h"
#include "transactions.h"
#include <vector>

using namespace tinyxml2;
using namespace std;

class parser{
 public:
  string createResponse();
  string parsexmlTop(const char * buffer);
  vector<account> createAccount(const char * buffer);
  transactions parseTransec(const char * buffer);
  string createSuccess(string id,string sym,int great);
  string createError(string id);
  
};
