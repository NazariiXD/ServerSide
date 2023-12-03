#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#define TEMPERATURE -3

#define STOCK_PRICE_MICROSOFT 374.51
#define STOCK_PRICE_GOOGLE 131.86
#define STOCK_PRICE_APPLE 191.24

#define EXCHANGE_DOLLAR_RATE 38.3
#define EXCHANGE_EURO_RATE 40.5
#define EXCHANGE_ZLOTY_RATE 9.3

DWORD WINAPI processClientRequests(LPVOID);
DWORD WINAPI updateWeatherForecast(LPVOID);
DWORD WINAPI updateExchangeRate(LPVOID);
DWORD WINAPI updateSharesRate(LPVOID);

void writeToFile(const string& fileName, const string& data);
double getRandomValue(int randomInterval);

string randomWeatherForecast();
string randomSharePrice();
string randomExchangeRate();