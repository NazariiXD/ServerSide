#include "utilityFunctions.h"

extern HANDLE mutex;
extern string sharesRate;
extern string exchangeRate;
extern string weatherForecast;


void writeToFile(const string& fileName, const string& data) {
    ofstream file(fileName, ios_base::app);
    if (file.is_open()) {
        file << __TIME__ + '\n' + data + '\n';
        file.close();
    }
}
double getRandomValue(int randomInterval) {
    double sign = (rand() % 2 == 0) ? 1.0 : -1.0;
    return sign * (static_cast<double>(rand() % randomInterval) + static_cast<double>(rand() % 100) / 100);
}

string randomWeatherForecast() {
    const int RANDOM_INTERVAL = 3;

    double temperature = TEMPERATURE + getRandomValue(RANDOM_INTERVAL);
    string str = " Temperature:  : " + to_string(temperature) + " °C";

    writeToFile("Weather.txt", str);

    return str;
}
string randomSharePrice() {
    const int RANDOM_INTERVAL = 3;

    double microsoftShare = STOCK_PRICE_MICROSOFT + getRandomValue(RANDOM_INTERVAL);
    double googleShare = STOCK_PRICE_GOOGLE + getRandomValue(RANDOM_INTERVAL);
    double appleShare = STOCK_PRICE_APPLE + getRandomValue(RANDOM_INTERVAL);

    string str = " Shares Microsoft:  : " + to_string(microsoftShare) +
        "\n Shares Google:  : " + to_string(googleShare) +
        "\n Shares Apple:  : " + to_string(appleShare);

    writeToFile("Shares.txt", str);

    return str;
}
string randomExchangeRate() {
    const int RANDOM_INTERVAL = 5;

    double dollarRate = EXCHANGE_DOLLAR_RATE + getRandomValue(RANDOM_INTERVAL);
    double euroRate = EXCHANGE_EURO_RATE + getRandomValue(RANDOM_INTERVAL);
    double zlotyRate = EXCHANGE_ZLOTY_RATE + getRandomValue(RANDOM_INTERVAL / 2);

    string str = " Dollar rate : " + to_string(dollarRate) +
        "\n Euro rate : " + to_string(euroRate) +
        "\n Zloty rate : " + to_string(zlotyRate);

    writeToFile("Rate.txt", str);

    return str;
}

DWORD WINAPI processClientRequests(LPVOID param) {
    SOCKET clientSocket = *reinterpret_cast<SOCKET*>(param);

    while (true) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Get data from client
        if (recv(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            cerr << "Receive failed" << endl;
            closesocket(clientSocket);
            continue;
        }

        cout << " " << __TIME__ << " Received message from client: " << buffer << std::endl;

        
        //Sending a response to the client
        const char* response = "Server received your message.";
        send(clientSocket, response, strlen(response), 0);
    }
    closesocket(clientSocket);

    return 0;
}

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