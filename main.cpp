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

  createAccount(200, "1234", C);
  createAccount(300, "2345", C);

  createPosition("1234", "BOA", 100, C);
  createPosition("2345", "BOA", 100, C);

  createOpen("1234", 13, 2, "BOA", 1, C);
  sleep(2);
  createOpen("1234", 21, -2, "BOA", 2, C);
  sleep(2);
  createOpen("2345", 23, 10, "BOA", 1, C);
  sleep(2);
  createOpen("1234", 22, -12, "BOA", 2, C);

  matchOneOrder(C, "3");
  // string open1 = "1";
  // cancel(open1, C);
}
