#include "utilityFunctions.h"
#include <string>
using namespace std;

extern HANDLE mutex;
extern string sharesRate;
extern string exchangeRate;
extern string weatherForecast;

DWORD WINAPI updateWeatherForecast(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        //weatherForecast = generateWeatherForecast(); //������������� �����
        ReleaseMutex(mutex);
        Sleep(7000);
    }
    return 0;
}
DWORD WINAPI updateExchangeRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        //exchangeRate = generateExchangeRate(); //������������� �����
        ReleaseMutex(mutex);
        Sleep(6000);
    }
    return 0;
}
DWORD WINAPI updateSharesRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        //sharesRate = generateSharesRate(); //������������� �����
        ReleaseMutex(mutex);
        Sleep(5000); //��� ��� ���� ������ ����. ��������� ���� ���� n ��.
    }
    return 0;
}