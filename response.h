#include <iostream>
using namespace std;
class response{
 public:
  bool open;
  string shares_o;
  
  bool executed;
  string shares_e;
  string price_e;
  string time_e; 
  
  bool cancel;
  string shares_c;
  string time_c;

  response(){
    open = false;
    shares_o = "";

    executed = false;
    shares_e = "";
    price_e = "";
    time_e = "";

    cancel = false;
    shares_c = "";
    time_c = "";
  }
};
