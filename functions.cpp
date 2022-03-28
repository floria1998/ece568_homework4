#include <fstream>
#include <ios>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <type_traits>
#include "functions.h"

using namespace std;
using namespace pqxx;


void database::openDatabase(connection **C) {
  try {
    *C = new connection("dbname=postgresbase user=postgres password=passw0rd");
    if ((*C)->is_open()) {
      cout << "Opened database successfully: " << (*C)->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return;
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return;
  }
}

void database::createTable(connection *C, string file) {
  const char *fileName = file.c_str();
  ifstream thefile(fileName);
  string query, line;
  if (thefile.is_open()) {
    while (getline(thefile, line)) {
      query += line;
    }
    thefile.close();
    work W(*C);
    W.exec(query);
    W.commit();
  } else {
    cerr << "Failed to open the file!" << endl;
    return;
  }
  return;
}

void database::dropTable(connection *C, string table) {
  string que = "DROP TABLE IF EXISTS " + table + " CASCADE;";
  cout<<table<<endl;
  work W(*C);
  W.exec(que);
  W.commit();
}

bool database::checkAccountExist(const string &id, connection *C) {
  work W(*C);
  string sql = "SELECT * FROM USER_TB WHERE ACCOUNT_ID=" + id + ";";
  result R(W.exec(sql));
  W.commit();
  // if not exist, return false
  if (R.size() == 0) {
    return false;
  } else {
    return true;
  }
}

int database::deleteFromOpen(const string &id, connection *C) {
  work W(*C);
  stringstream sql;
  sql << "DELETE FROM OPEN_TB WHERE OPEN_ID=" << W.quote(id) << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::addToExecuted(const string &buyer_id, const string &seller_id, double price,
                  int amount, string symbol, connection *C) {
  work W(*C);
  stringstream sql;
  time_t curr_time = time(NULL);
  sql << "INSERT INTO EXECUTED_TB "
         "(BUYER_ID,SELLER_ID,PRICE,AMOUNT,EXECUTED_TIME,SYMBOL) VALUES "
      << "(" << W.quote(to_string(buyer_id)) << ", "
      << W.quote(to_string(seller_id)) << ", " << W.quote(to_string(price))
      << ", " << W.quote(to_string(amount)) << ", "
      << W.quote(to_string(curr_time)) << ", " << W.quote(symbol) << ");";
  result R(W.exec(sql.str()));
  W.commit();

  nontransaction N(*C);
  stringstream sql2;
  sql2 << "SELECT EXECUTED_ID FROM EXECUTED_TB WHERE EXECUTED_TIME="
       << curr_time << ";";
  result R1(N.exec(sql2.str()));
  int executed_id = R1[0]["EXECUTED_ID"].as<int>();
  return executed_id;
}

int database::updateOpenAmount(const string &id, int new_amount, connection *C) {
  work W(*C);
  stringstream sql;
  sql << "UPDATE OPEN_TB SET AMOUNT =" << W.quote(new_amount)
      << " WHERE ACCOUNT_ID =" << W.quote(id) << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::payBackBuyer(int id, double toadd, connection *C) {
  work W(*C);
  stringstream sql1;
  sql1 << "SELECT BALANCE FROM USER_TB WHERE ACCOUNT_ID=" << id << ";";
  result R(W.exec(sql1.str()));
  double balance = R[0]["BALANCE"].as<double>();
  double newbalance = balance + toadd;
  stringstream sql;
  sql << "UPDATE USER_TB SET BALANCE =" << W.quote(newbalance)
      << " WHERE ACCOUNT_ID=" << id << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::createAccount(double balance, string id, connection *C) {
  bool exist = checkAccountExist(id, C);
  if (exist == true) {
    return 0;
  }
  work W(*C);
  stringstream sql;
  sql << "INSERT INTO USER_TB (ACCOUNT_ID, BALANCE) VALUES (" << W.quote(id)
      << ", " << W.quote(balance) << ");";

  W.exec(sql.str());
  W.commit();
  return 1;
}
/* checkPositionExist(string &id, connection *C, string symbol) {
  nontransaction N(*C);
  string sql = "SELECT * FROM POSITION WHERE ACCOUNT_ID=" + N.quote(id) +
               " AND SYMBOL=" + N.quote(symbol) + ";";
  result R(N.exec(sql));
  return R.size();
}
*/
int database::createPosition(const string &id, const string &symbol, int amount,
                   connection *C) {
  bool account_exist = checkAccountExist(id, C);
  if (account_exist == false) {
    return 0;
  }
  work W(*C);
  string sql = "SELECT * FROM POSITION_TB WHERE POSITION_TB.ACCOUNT_ID=" + id +
               " AND SYMBOL=" + W.quote(symbol) + ";";
  result R(W.exec(sql));
  int num = R.size();
  // work W(*C);
  if (num == 0) {
    stringstream sql2;
    sql2 << "INSERT INTO POSITION_TB (SYMBOL,ACCOUNT_ID,AMOUNT) VALUES ("
         << W.quote(symbol) << ", " << W.quote(id) << ","
         << W.quote(to_string(amount)) << ");";

    W.exec(sql2.str());
    W.commit();
    return 1;
  } else {
    int myamount = R[0]["AMOUNT"].as<int>() + amount;
    stringstream sql2;
    sql2 << "UPDATE POSITION_TB SET AMOUNT =" << W.quote(to_string(myamount))
         << " WHERE ACCOUNT_ID=" << id << " AND SYMBOL =" << W.quote(symbol)
         << ";";
    W.exec(sql2.str());
    W.commit();
    return 1;
  }
}

int database::updateTranEXEOnly(const string &id, int exe_id, connection *C) {
  work W(*C);
  stringstream sql;

  sql << "UPDATE TRANSACTION_TB SET EXECUTED_ID =" << exe_id
      << ", OPEN_ID = NULL WHERE TRANSACTION_ID=" << id << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::updateTranEXEandOpen(const string &id, const string &exe_id,
                         connection *C) {
  work W(*C);
  stringstream sql;

  sql << "UPDATE TRANSACTION_TB SET EXECUTED_ID =" << exe_id
      << " WHERE TRANSACTION_ID=" << id << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::updateAccount(string id, double price, int amount, connection *C) {
  work W(*C);
  stringstream sql;
  double new_balance = price * amount;

  sql << "UPDATE USER_TB SET BALANCE =" << W.quote(to_string(new_balance))
      << " WHERE ACCOUNT_ID=" << id << ";";
  W.exec(sql.str());
  W.commit();
  return 1;
}

int database::createOpen(string id, double price, int amount, string symbol, int type,
               connection *C) {
  // place the buyer's order
  if (type == 1) {
    double deduct = price * (double)amount;
    stringstream sql_buyer;
    work N3(*C);
    sql_buyer << "SELECT BALANCE FROM USER_TB WHERE ACCOUNT_ID=" << id << ";";
    result R(N3.exec(sql_buyer.str()));
    N3.commit();
    double balance = R[0]["BALANCE"].as<double>();
    if (balance > deduct) {
      double remain = balance - deduct;
      work W4(*C);
      stringstream sql_deduct;
      sql_deduct << "UPDATE USER_TB SET BALANCE=" << W4.quote(to_string(remain))
                 << " WHERE ACCOUNT_ID=" << id << ";";
      W4.exec(sql_deduct.str());
      W4.commit();
    } else {
      cerr << "There is not enough balance in user's account" << endl;
      return -1;
    }
  }
  // place the seller's order
  else if (type == 2) {
    stringstream sql_seller;
    work N4(*C);
    sql_seller << "SELECT AMOUNT FROM POSITION_TB WHERE ACCOUNT_ID=" << id
               << ";";
    result R(N4.exec(sql_seller.str()));
    N4.commit();
    int share = -(R[0]["AMOUNT"].as<int>());
    if (amount > share) {
      int remain = amount - share;
      work W5(*C);
      stringstream sql_deduct;
      sql_deduct << "UPDATE POSITION_TB SET AMOUNT="
                 << W5.quote(to_string(remain)) << " WHERE ACCOUNT_ID=" << id
                 << " AND SYMBOL=" << W5.quote(symbol) << ";";
      W5.exec(sql_deduct.str());
      W5.commit();
    } else {
      cerr << "There is not enough share in seller's account" << endl;
      return -1;
    }
  }
  // insert the order into open table
  time_t openTime = time(NULL);
  stringstream sql;
  work W(*C);
  sql << "INSERT INTO OPEN_TB "
         "(ACCOUNT_ID,PRICE,AMOUNT,OPEN_TIME,SYMBOL,TRAN_TYPE) "
      << "VALUES (" << W.quote(id) << ", " << W.quote(to_string(price)) << ", "
      << W.quote(to_string(amount)) << ", " << W.quote(to_string(openTime))
      << ", " << W.quote(symbol) << ", " << W.quote(to_string(type)) << ");";

  result R(W.exec(sql.str()));
  W.commit();

  work N(*C);
  stringstream sql_find;
  sql_find << "SELECT OPEN_ID FROM OPEN_TB WHERE OPEN_TIME="
           << to_string(openTime) << ";";
  result R3(N.exec(sql_find));
  int open_id = R3[0]["OPEN_ID"].as<int>();
  N.commit();

  work W2(*C);
  stringstream sql_insert;
  sql_insert << "INSERT INTO TRANSACTION_TB (OPEN_ID) VALUES ("
             << W2.quote(to_string(open_id)) << ");";
  result R2(W2.exec(sql_insert));
  W2.commit();

  work N2(*C);
  stringstream sql_tranid;
  sql_tranid << "SELECT TRANSACTION_ID FROM TRANSACTION_TB WHERE OPEN_ID="
             << N2.quote(to_string(open_id)) << ";";
  result R4(N2.exec(sql_tranid));
  N2.commit();
  int tran_id = R4[0]["TRANSACTION_ID"].as<int>();
  return tran_id;
}

// match a transaction
bool database::matchOneOrder(connection *C, const string &tran_id) {
  work W(*C);

  stringstream sql1;
  sql1 << "SELECT OPEN_ID FROM TRANSACTION_TB WHERE TRANSACTION_ID=" << tran_id
       << ";";
  result R(W.exec(sql1.str()));
  int open_id = R[0]["OPEN_ID"].as<int>();
  // get the information of the account that should be matched
  stringstream sql2;
  sql2 << "SELECT ACCOUNT_ID,AMOUNT,PRICE,SYMBOL,TRAN_TYPE,OPEN_TIME FROM "
          "OPEN_TB "
          "WHERE OPEN_ID="
       << open_id << ";";
  result R2(W.exec(sql2.str()));
  int account_id = R2[0]["ACCOUNT_ID"].as<int>();
  double price = R2[0]["PRICE"].as<double>();
  int amount = R2[0]["AMOUNT"].as<int>();
  string symbol = R2[0]["SYMBOL"].as<string>();
  int type = R2[0]["TRAN_TYPE"].as<int>();
  long open_time = R2[0]["OPEN_TIME"].as<long>();
  // if the order is the buyer order
  if (type == 1) {
    stringstream sql3;
    sql3 << "SELECT "
            "OPEN_TB.OPEN_ID,OPEN_TB.ACCOUNT_ID,PRICE,SYMBOL,AMOUNT,OPEN_TIME,"
            "TRAN_TYPE,"
            "TRANSACTION_ID FROM "
            "OPEN_TB, TRANSACTION_TB WHERE "
            "TRANSACTION_TB.OPEN_ID=OPEN_TB.OPEN_ID AND "
            "ACCOUNT_ID !="
         << account_id << " AND TRAN_TYPE=" << 2 << " AND PRICE<" << price
         << " AND SYMBOL=" << W.quote(symbol) << " ORDER BY PRICE ASC;";
    result R3(W.exec(sql3.str()));
    int num = R3.size();
    if (num == 0) {
      cout << "There is no matching order!" << endl;
      return false;
    }
    W.commit();
    // all the orders that match
    for (result::const_iterator k = R3.begin(); k != R3.end(); k++) {
      int sell_open_id = k["OPEN_ID"].as<int>();
      int sell_transaction_id = k["TRANSACTION_ID"].as<int>();
      int sell_account_id = k["ACCOUNT_ID"].as<int>();
      double sell_price = k["PRICE"].as<double>();
      int sell_amount = -(k["AMOUNT"].as<int>());
      string sell_symbol = k["SYMBOL"].as<string>();
      int sell_type = k["TRAN_TYPE"].as<int>();
      long sell_open_time = k["OPEN_TIME"].as<long>();
      double tran_price = open_time > sell_open_time ? sell_price : price;
      if (amount < sell_amount) {
        int remain = sell_amount - amount;
        // update seller account;
        updateAccount(to_string(sell_account_id), tran_price, amount, C);
        // update buyer account
        createPosition(to_string(account_id), symbol, amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * amount;
          payBackBuyer(account_id, toadd, C);
        }
        // totally delete the buyer from the open table and add to the executed
        // table update the amount of seller in the open table
        deleteFromOpen(to_string(open_id), C);
        updateOpenAmount(to_string(sell_account_id), remain, C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, amount, symbol, C);
        // update the transaction table, set buyer's open_id to null and set the
        // executed_id
        updateTranEXEOnly(tran_id, exe_id, C);
        // update the transaction table, set seller's executed_id
        updateTranEXEandOpen(to_string(sell_transaction_id), to_string(exe_id),
                             C);
        break;
      } else if (amount > sell_amount) {
        amount = amount - sell_amount;
        // update the seller account, buyer account
        updateAccount(to_string(sell_account_id), tran_price, sell_amount, C);
        createPosition(to_string(account_id), symbol, sell_amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * sell_amount;
          payBackBuyer(account_id, toadd, C);
        }
        // totally delete the sell from the open table and add to the executed
        // table update the amount of buyer in the open table, add part that
        // matched to the executed table
        deleteFromOpen(to_string(sell_open_id), C);
        updateOpenAmount(to_string(account_id), amount, C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, sell_amount, symbol, C);
        // update the transaction table, set seller's open_id to null and set
        // the executed_id
        updateTranEXEOnly(to_string(sell_transaction_id), exe_id, C);
        updateTranEXEandOpen(tran_id, to_string(exe_id), C);
        continue;
        // update the transaction table, set buyer's executed_id
      } else {
        updateAccount(to_string(sell_account_id), tran_price, amount, C);
        createPosition(to_string(account_id), symbol, amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * sell_amount;
          payBackBuyer(account_id, toadd, C);
        }
        deleteFromOpen(to_string(sell_open_id), C);
        deleteFromOpen(to_string(open_id), C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, sell_amount, symbol, C);
        updateTranEXEOnly(to_string(sell_transaction_id), exe_id, C);
        updateTranEXEOnly(tran_id, exe_id, C);
        break;
      }
    }
  }
  // the order is sell order
  else {
    amount = -amount;
    stringstream sql3;
    // select all the buy orders that match
    sql3 << "SELECT OPEN_ID,ACCOUNT_ID,PRICE,AMOUNT,TIME, TRANSACTION_ID FROM "
            "OPEN_TB, TRANSACTION_TB WHERE "
            "TRANSACTION_TB.OPEN_ID=OPEN_TB.OPEN_ID AND "
            "ACCOUNT_ID !="
         << account_id << " AND TRAN_TYPE=" << 1 << " AND PRICE>" << price
         << " AND SYMBOL=" << W.quote(symbol) << " ORDER BY PRICE DESC;";
    result R3(W.exec(sql3.str()));
    int num = R3.size();
    if (num == 0) {
      return false;
    }
    // iterate through all the buy orders
    for (result::const_iterator k = R3.begin(); k != R3.begin(); k++) {
      int sell_open_id = k["OPEN_ID"].as<int>();
      int sell_transaction_id = k["TRANSACTION_ID"].as<int>();
      int sell_account_id = k["ACCOUNT_ID"].as<int>();
      double sell_price = k["PRICE"].as<double>();
      int sell_amount = k["AMOUNT"].as<int>();
      string sell_symbol = k["SYMBOL"].as<string>();
      int sell_type = k["TRAN_TYPE"].as<int>();
      long sell_open_time = k["OPEN_TIME"].as<long>();
      double tran_price = open_time > sell_open_time ? sell_price : price;

      if (amount < sell_amount) {
        int remain = sell_amount - amount;
        // update seller account;
        updateAccount(to_string(sell_account_id), tran_price, amount, C);
        // update buyer account
        createPosition(to_string(account_id), symbol, amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * amount;
          payBackBuyer(account_id, toadd, C);
        }
        // totally delete the buyer from the open table and add to the executed
        // table update the amount of seller in the open table
        deleteFromOpen(to_string(open_id), C);
        updateOpenAmount(to_string(sell_account_id), remain, C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, amount, symbol, C);
        // update the transaction table, set buyer's open_id to null and set the
        // executed_id
        updateTranEXEOnly(tran_id, exe_id, C);
        // update the transaction table, set seller's executed_id
        updateTranEXEandOpen(to_string(sell_transaction_id), to_string(exe_id),
                             C);
        break;
      } else if (amount > sell_amount) {
        amount = amount - sell_amount;
        // update the seller account, buyer account
        updateAccount(to_string(sell_account_id), tran_price, sell_amount, C);
        createPosition(to_string(account_id), symbol, sell_amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * sell_amount;
          payBackBuyer(account_id, toadd, C);
        }
        // totally delete the sell from the open table and add to the executed
        // table update the amount of buyer in the open table, add part that
        // matched to the executed table
        deleteFromOpen(to_string(sell_open_id), C);
        updateOpenAmount(to_string(account_id), amount, C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, sell_amount, symbol, C);
        // update the transaction table, set seller's open_id to null and set
        // the executed_id
        updateTranEXEOnly(to_string(sell_transaction_id), exe_id, C);
        updateTranEXEandOpen(tran_id, to_string(exe_id), C);
        continue;
        // update the transaction table, set buyer's executed_id
      } else {
        updateAccount(to_string(sell_account_id), tran_price, amount, C);
        createPosition(to_string(account_id), symbol, amount, C);
        if (tran_price < price) {
          double difPirce = price - tran_price;
          double toadd = difPirce * sell_amount;
          payBackBuyer(account_id, toadd, C);
        }
        deleteFromOpen(to_string(sell_open_id), C);
        deleteFromOpen(to_string(open_id), C);
        int exe_id =
            addToExecuted(to_string(account_id), to_string(sell_account_id),
                          tran_price, sell_amount, symbol, C);
        updateTranEXEOnly(to_string(sell_transaction_id), exe_id, C);
        updateTranEXEOnly(tran_id, exe_id, C);
        break;
      }
    }
  }
  return 1;
}

void database::updateTranCancel(string &id,string cancel_id,connection *C)
{
  work W(*C);
  stringstream sql;

  sql<<"UPDATE TRANSACTION_TB SET CANCEL_ID="<<cancel_id<<"WHERE TRANSACTION_ID="<<id<<";";
  W.exec(sql.str());
  W.commit();
  // return 1;
}

int database::cancel(string &id, connection *C) {
  work W(*C);
  stringstream sql;
  sql << "SELECT OPEN_TB.OPEN_ID,SYMBOL,ACCOUNT_ID,TRAN_TYPE,AMOUNT,PRICE FROM "
         "OPEN_TB,TRANSACTION_TB WHERE TRANSACTION_TB.OPEN_ID=OPEN_TB.OPEN_ID "
         "AND "
         "TRANSACTION_ID="
      << id << ";";
  result R(W.exec(sql));
  W.commit();
  int open_id = R[0]["OPEN_ID"].as<int>();
  int account_id = R[0]["ACCOUNT_ID"].as<int>();
  int type = R[0]["TRAN_TYPE"].as<int>();
  int amount = R[0]["AMOUNT"].as<int>();
  string symbol = R[0]["SYMBOL"].as<string>();
  double price = R[0]["PRICE"].as<double>();
  
  deleteFromOpen(to_string(open_id), C);

  if (type == 1) {
    double payback = amount * price;
    payBackBuyer(account_id, payback, C);
  } else {
    amount = -amount;
    createPosition(to_string(account_id), symbol, amount, C);
  }

  work W2(*C);
  time_t curr_time = time(NULL);
  stringstream sql2;
  sql2 << "INSERT INTO CANCEL_TB (ACCOUNT_ID,AMOUNT,CANCEL_TIME) VALUES ("
       << W.quote(account_id) << ", " << W.quote(amount) << ", "
       << W.quote(curr_time) << ");";
  W2.exec(sql2);
  
  W2.commit();

  work W3(*C);
  stringstream sql3;
   sql3 << "SELECT CANCEL_ID FROM CANCEL_TB WHERE CANCEL_TIME=" << curr_time
       << ";";
  result R3(W3.exec(sql3));
  W3.commit();

  int cancel_id = R3[0]["CANCEL_ID"].as<int>();

  updateTranCancel(id, to_string(cancel_id), C);

  return 1;
 }

response database::queryDB(string user_id,string query_id,connection *C)
{
  // find canceled
  response a;
  nontransaction N(*C);
  stringstream sql;
  sql << "SELECT * FROM "
         "TRANSACTION_TB WHERE "
         "TRANSACTION_ID ="
      << query_id << ";";
  result R(N.exec(sql));
  string open_id = "";
  string executed_id = "";
  string cancel_id = "";
  for (result::const_iterator c = R.begin();c!=R.end();c++)
  {
    if (!(c[1].is_null()))
    	{
    	  open_id = c[1].as<string>();
	}
      
    if (!(c[2].is_null()))
	  	{
	  open_id = c[2].as<string>();
	  	}
      
    if (!(c[3].is_null()))
	   {
	  open_id = c[3].as<string>();
	  }
   }
  
  // W.commit();
  // find cancel
  if (cancel_id!="")
    {
      stringstream m;
       m << "SELECT AMOUNT,CANCEL_TIME FROM "
         "CANCEL_TB WHERE "
         "ACCOUNT_ID ="
	   << user_id << " AND CANCEL_ID ="<<cancel_id<<";";
     result R(N.exec(m));
     a.cancel = true;
     for (result::const_iterator c = R.begin();c!=R.end();c++)
       {
	 //    a.cancel = true;
	 a.shares_c = c[0].as<string>();
	 a.time_c = c[1].as<string>();
       }
    }
  
  // find open  
  if (open_id!= "")
    {
       stringstream m;
       m << "SELECT AMOUNT FROM "
         "OPEN_TB WHERE "
         "ACCOUNT_ID ="
	   << user_id << " AND OPEN_ID ="<<open_id<<";";
     result R(N.exec(m));
     a.open = true;
     for (result::const_iterator c = R.begin();c!=R.end();c++)
       {  
	 a.shares_o = c[0].as<string>();
       }
    }
  // find executed
  if (executed_id != "")
  {
       stringstream m;
       m << "SELECT AMOUNT,PRICE,EXECUTED_TIME FROM "
         "EXECUTED_TB WHERE ("
         "BUYER_ID ="
	 << user_id << "OR SELLER_ID="<<user_id<<" ) AND EXECUTED_ID ="<<executed_id<<";";
     result R(N.exec(m));
     a.executed = true;
     for (result::const_iterator c = R.begin();c!=R.end();c++)
       {
	 a.shares_e = c[0].as<string>();
	 a.price_e = c[1].as<string>();
	 a.time_e = c[2].as<string>();
       }
  }
  //  cout<<a.open<<endl;
  return a;
}
