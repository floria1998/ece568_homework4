#include <iostream>
using namespace std;
class response{
 public:
  int open;
  string shares_o;
  
  int executed;
  string shares_e;
  string price_e;
  string time_e; 
  
  int cancel;
  string shares_c;
  string time_c;

  response(){
    open = 0;
    shares_o = "";

    executed = 0;
    shares_e = "";
    price_e = "";
    time_e = "";

    cancel = 0;
    shares_c = "";
    time_c = "";
  }
};
