#include "functions.h"
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <unistd.h>
using namespace std;
using namespace pqxx;

int main(int argc, char *argv[]) {
  connection *C;
  database p;
  // openDatabase(C);

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=postgresbase user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return 1;
  }
  /*<<<<<<< HEAD
  dropTable(C, "USER_TB");
  dropTable(C, "POSITION_TB");
  dropTable(C, "TRANSACTION_TB");
  dropTable(C, "OPEN_TB");
  dropTable(C, "EXECUTED_TB");
  dropTable(C, "CANCEL_TB");
  createTable(C, "createTable.sql");

  createAccount(200, "1234", C);
  createAccount(300, "2345", C);

  createPosition("1234", "BOA", 100, C);
  createPosition("2345", "BOA", 100, C);

  createOpen("1234", 24, 2, "BOA", 1, C);
  sleep(2);
  createOpen("1234", 21, -2, "BOA", 2, C);
  sleep(2);
  createOpen("1234", 23, 3, "BOA", 1, C);
  sleep(2);
  createOpen("2345", 22, -7, "BOA", 2, C);

  matchOneOrder(C, "4");
  string opn1 = "2";
  cancel(opn1, C);
  =======*/
  p.dropTable(C, "USER_TB");
  p.dropTable(C, "POSITION_TB");
  p.dropTable(C, "TRANSACTION_TB");
  p.dropTable(C, "OPEN_TB");
  p.dropTable(C, "EXECUTED_TB");
  p.dropTable(C, "CANCEL_TB");
  p.createTable(C, "createTable.sql");
  double balance1 = 200;
  string id1 = "1234";
  double balance2 = 300;
  string id2 = "2345";
  p.createAccount(balance1, id1, C);
  p.createAccount(balance2, id2, C);
  string symbol = "BOA";
  int amount = 100;
  p.createPosition(id1, symbol, amount, C);
  int add = 200;
  p.createPosition(id1, symbol, add, C);
  p.createPosition(id2, symbol, 300, C);
  double price = 10.1;
  int amount1 = 1;
  int type = 1;
  p.createOpen(id1, price, amount1, symbol, type, C);
  sleep(2);
  double price2 = 22;
  int amount2 = -2;
  int type2 = 2;
  p.createOpen(id1, price2, amount2, symbol, type2, C);
  sleep(2);
  double price3 = 23;
  int amount3 = 10;
 int x =   p.createOpen(id2, price3, amount3, symbol, type, C);
 cout<<x<<endl;
  string tran_id = "3";
  p.matchOneOrder(C, tran_id);
  string open1 = "1";
  p.cancel(open1, C);
  //  string tran_id = "3";
  string user_id = "1";
  string tran_id2 = "2";
  p.queryDB(user_id,tran_id2,C);
  //>>>>>>> parser
}
