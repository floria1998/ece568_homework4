#include "functions.h"
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <unistd.h>
using namespace std;
using namespace pqxx;

int main(int argc, char *argv[]) {
  connection *C;
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
  dropTable(C, "USER_TB");
  dropTable(C, "POSITION_TB");
  dropTable(C, "TRANSACTION_TB");
  dropTable(C, "OPEN_TB");
  dropTable(C, "EXECUTED_TB");
  dropTable(C, "CANCEL_TB");
  createTable(C, "createTable.sql");
  double balance1 = 200;
  string id1 = "1234";
  double balance2 = 300;
  string id2 = "2345";
  createAccount(balance1, id1, C);
  createAccount(balance2, id2, C);
  string symbol = "BOA";
  int amount = 100;
  createPosition(id1, symbol, amount, C);
  int add = 200;
  createPosition(id1, symbol, add, C);
  createPosition(id2, symbol, 300, C);
  double price = 10.1;
  int amount1 = 1;
  int type = 1;
  createOpen(id1, price, amount1, symbol, type, C);
  sleep(2);
  double price2 = 22;
  int amount2 = -2;
  int type2 = 2;
  createOpen(id1, price2, amount2, symbol, type2, C);
  sleep(2);
  double price3 = 23;
  int amount3 = 10;
  createOpen(id2, price3, amount3, symbol, type, C);
  string tran_id = "3";
  matchOneOrder(C, tran_id);
  string open1 = "1";
  cancel(open1, C);
}