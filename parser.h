#include <cstring>
#include "tinyxml2.h"
#include "string.h"
#include <iostream>
#include "account.h"
#include "transactions.h"

using namespace tinyxml2;

class parser{
 public:
  std::string createResponse();
  std::string parsexmlTop(const char * buffer);
  account createAccount(const char * buffer);
  transactions parseTransec(const char * buffer);
};
