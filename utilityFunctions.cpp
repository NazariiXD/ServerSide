#include "utilityFunctions.h"

extern HANDLE mutex;
std::string sharesRate;
std::string exchangeRate;
std::string weatherForecast;
std::vector<clientData> clients;

void writeToFile(const std::string& fileName, const std::string& data) {
    std::ofstream file(fileName, std::ios_base::app);
    if (file.is_open()) {
        file << __TIME__ + '\n' + data + '\n';
        file.close();
    }
}
double getRandomValue(int randomInterval) {
    double sign = (rand() % 2 == 0) ? 1.0 : -1.0;
    return sign * (static_cast<double>(rand() % randomInterval) + static_cast<double>(rand() % 100) / 100);
}

std::string randomWeatherForecast() {
    const int RANDOM_INTERVAL = 3;
    double temperature = TEMPERATURE + getRandomValue(RANDOM_INTERVAL);
    std::string str = " Temperature:  : " + std::to_string(temperature) + " °C";
    writeToFile("Weather.txt", __TIME__);
    writeToFile("Weather.txt", str + "\n");
    return str;
}
std::string randomSharePrice() {
    const int RANDOM_INTERVAL = 50;
    double microsoftShare = STOCK_PRICE_MICROSOFT + getRandomValue(RANDOM_INTERVAL);
    double googleShare = STOCK_PRICE_GOOGLE + getRandomValue(RANDOM_INTERVAL);
    double appleShare = STOCK_PRICE_APPLE + getRandomValue(RANDOM_INTERVAL);
    std::string str = " Shares Microsoft:  : " + std::to_string(microsoftShare) +
        "\n Shares Google:  : " + std::to_string(googleShare) +
        "\n Shares Apple:  : " + std::to_string(appleShare);

    writeToFile("Shares.txt", __TIME__);
    writeToFile("Shares.txt", str + "\n");
    return str;
}
std::string randomExchangeRate() {
    const int RANDOM_INTERVAL = 2;
    double dollarRate = EXCHANGE_DOLLAR_RATE + getRandomValue(RANDOM_INTERVAL);
    double euroRate = EXCHANGE_EURO_RATE + getRandomValue(RANDOM_INTERVAL);
    double zlotyRate = EXCHANGE_ZLOTY_RATE + getRandomValue(RANDOM_INTERVAL / 2);
    std::string str = " Dollar rate : " + std::to_string(dollarRate) +
        "\n Euro rate : " + std::to_string(euroRate) +
        "\n Zloty rate : " + std::to_string(zlotyRate);
    writeToFile("Rate.txt", __TIME__);
    writeToFile("Rate.txt", str + "\n");
    return str;
}

DWORD WINAPI manageClientSubscription(LPVOID param) {
    SOCKET clientSocket = *((SOCKET*)(param)); //why use reinterpret_cast?
    while (true) {
        short int subscriptionMask = 0;

        // Get data from client
        if (recv(clientSocket, (char *)&subscriptionMask, sizeof(short int), 0) == SOCKET_ERROR) {
            std::cerr << " Receive failed (Lost connection to client)\n";
            WaitForSingleObject(mutex, INFINITE);
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if disconnected
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << __TIME__ << " Deleted disconnected client from subscription list\n";
                }
            }
            ReleaseMutex(mutex);
            closesocket(clientSocket);
            return 1;
        }
        std::cout << " " << __TIME__ << " Received message from client: " << std::bitset<3>(subscriptionMask) << "\n";
        clientData client;
        client.socket = clientSocket;
        client.subscription = subscriptionMask;
        int present = 0;
        WaitForSingleObject(mutex, INFINITE);
        if (subscriptionMask == 0) {
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if unsubscribed
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << __TIME__ << " Deleted unsubscribed client from list\n";
                }
            }
        }
        else {
            for (int i = 0; i < clients.size(); i++) {   // Add/update subscription
                if (clientSocket == clients[i].socket) {
                    clients[i].subscription = client.subscription;
                    present = 1;
                    std::cout << " " << __TIME__ << " Updated client subscription to " << std::bitset<3>(subscriptionMask) << "\n";
                    break;
                }
            }
            if (present == 0) {
                clients.push_back(client);
                std::cout << " " << __TIME__ << " Added client with" << std::bitset<3>(subscriptionMask) << " subscription mask\n";
            }
        }
        ReleaseMutex(mutex);
        //Sending a response to the client
        const char* response = "Server received your subscription data.";
        send(clientSocket, response, strlen(response), 0);
    }
    closesocket(clientSocket);
    return 0;
}

DWORD WINAPI sendWeatherForecast(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        weatherForecast = randomWeatherForecast();
        for (int i = 0; i < clients.size(); i++) {
            send(clients[i].socket, weatherForecast.c_str(), strlen(weatherForecast.c_str()), 0);
            // TODO: handle failed send
        }
        ReleaseMutex(mutex);
        Sleep(1000);
    }
    return 0;
}
DWORD WINAPI sendExchangeRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        exchangeRate = randomExchangeRate();
        for (int i = 0; i < clients.size(); i++) {
            send(clients[i].socket, exchangeRate.c_str(), strlen(exchangeRate.c_str()), 0);
            // TODO: handle failed send
        }
        ReleaseMutex(mutex);
        Sleep(2500);
    }
    return 0;
}
DWORD WINAPI sendSharePrice(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        sharesRate = randomSharePrice();
        for (int i = 0; i < clients.size(); i++) {
            send(clients[i].socket, sharesRate.c_str(), strlen(sharesRate.c_str()), 0);
            // TODO: handle failed send
        }
        ReleaseMutex(mutex);
        Sleep(7500);
    }
    return 0;
}