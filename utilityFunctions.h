#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include <vector>
#include <chrono>
#include <iomanip>

#define TEMPERATURE 0

#define STOCK_PRICE_MICROSOFT 374.51
#define STOCK_PRICE_GOOGLE 131.86
#define STOCK_PRICE_APPLE 191.24

#define EXCHANGE_DOLLAR_RATE 37.45
#define EXCHANGE_EURO_RATE 40.5
#define EXCHANGE_ZLOTY_RATE 9.35

struct clientData {
	SOCKET socket;
	short int subscription;
};
struct mailingData {
	short int type;
	char message[200];
};
DWORD WINAPI manageClientSubscription(LPVOID);
DWORD WINAPI sendWeatherForecast(LPVOID);
DWORD WINAPI sendExchangeRate(LPVOID);
DWORD WINAPI sendSharePrice(LPVOID);

std::string getCurrentTime();
void writeToFile(const std::string& fileName, const std::string& data);
double getRandomValue(double randomInterval);

std::string randomWeatherForecast();
std::string randomExchangeRate();
std::string randomSharePrice();