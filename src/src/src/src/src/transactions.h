#include "string.h"
#include <cstring>

class transactions{
public: 
  std::string account_id;

  bool cancel;
  std::string cancel_id;
  
  bool query;
  std::string query_id;
  
  bool order;
  std::string symbol;
  std::string amount;
  std::string limit;
};
