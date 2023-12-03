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
        //weatherForecast = generateWeatherForecast(); //імплементацію треба
        ReleaseMutex(mutex);
        Sleep(7000);
    }
    return 0;
}
DWORD WINAPI updateExchangeRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        //exchangeRate = generateExchangeRate(); //імплементацію треба
        ReleaseMutex(mutex);
        Sleep(6000);
    }
    return 0;
}
DWORD WINAPI updateSharesRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        //sharesRate = generateSharesRate(); //імплементацію треба
        ReleaseMutex(mutex);
        Sleep(5000); //час хай буде менший поки. Оновлюємо інфу кожні n мс.
    }
    return 0;
}