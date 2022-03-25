//
// Created by HP on 2022/3/24.
//
#ifndef MAIN_CPP_FUNCTIONS_H
#define MAIN_CPP_FUNCTIONS_H

#include <fstream>
#include <ios>
#include <iostream>
#include <pqxx/connection.hxx>
#include <pqxx/nontransaction.hxx>
#include <pqxx/pqxx>
#include <string>
#include <type_traits>

using namespace std;
using namespace pqxx;

void openDatabase(connection *C);
void createTable(connection *C, string file);
void dropTable(connection *C, string table);
bool checkAccountExist(const string &id, connection *C);
int deleteFromOpen(const string &id, connection *C);
int addToExecuted(const string &buyer_id, const string &seller_id, double price,
                  int amount, string symbol, connection *C);
int createAccount(double balance, string id, connection *C);
int createPosition(const string &id, const string &symbol, int amount,
                   connection *C);
int updateAccount(string &id, double price, int amount, connection *C);
int createOpen(string id, double price, int amount, string symbol, int type,
               connection *C);
bool matchOneOrder(connection *C, const string &tran_id);
int updateOpenAmount(const string &id, int new_amount, connection *C);
int updateTranEXEOnly(const string &id, int exe_id, connection *C);
int updateTranEXEandOpen(const string &id, const string &exe_id, connection *C);
int payBackBuyer(int id, double toadd, connection *C);
int cancel(string &id, connection *C);

#endif // MAIN_CPP_STOCK_H
