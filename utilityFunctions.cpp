#include "utilityFunctions.h"

extern HANDLE mutex;
std::string sharesRate;
std::string exchangeRate;
std::string weatherForecast;

double sharesPriceVal[3] = {STOCK_PRICE_MICROSOFT, STOCK_PRICE_GOOGLE, STOCK_PRICE_APPLE};
double exchangeRateVal[3] = {EXCHANGE_DOLLAR_RATE, EXCHANGE_EURO_RATE, EXCHANGE_ZLOTY_RATE};
double temperature = TEMPERATURE;

std::vector<clientData> clients;

void writeToFile(const std::string& fileName, const std::string& data) {
    std::ofstream file(fileName, std::ios_base::app);
    if (file.is_open()) {
        file << __TIME__;
        file << "\n" + data + "\n";
        file.close();
    }
}
double getRandomValue(int randomInterval) {
    double sign = (rand() % 2 == 0) ? 1.0 : -1.0;
    return sign * (static_cast<double>(rand() % randomInterval) + static_cast<double>(rand() % 100) / 100);
}

std::string randomWeatherForecast() {
    const int RANDOM_INTERVAL = 3;
    double randomValue = getRandomValue(RANDOM_INTERVAL);
    if (temperature + randomValue > 40.0) {
        temperature -= 1.0;
    }
    else if (temperature + randomValue < -35.0) {
        temperature += 1.0;
    }
    else {
        temperature += randomValue;
    }
    std::string str = " Temperature : " + std::to_string(temperature) + " °C";

    writeToFile("Weather.txt", str);
    return str;
}
std::string randomSharePrice() {
    const int RANDOM_INTERVAL = 3;
    for (int i = 0; i < sizeof(sharesPriceVal)/sizeof(sharesPriceVal[0]); i++) {
        double randomValue = getRandomValue(RANDOM_INTERVAL);

        sharesPriceVal[i] = (sharesPriceVal[i] + randomValue > 0) ? (sharesPriceVal[i] + randomValue) : 0;
    }
    std::string str = " Shares Microsoft : " + std::to_string(sharesPriceVal[0]) +
        "\n Shares Google : " + std::to_string(sharesPriceVal[1]) +
        "\n Shares Apple : " + std::to_string(sharesPriceVal[2]);

    writeToFile("Shares.txt", str);
    return str;
}
std::string randomExchangeRate() {
    const int RANDOM_INTERVAL = 2;

    for (int i = 0; i < sizeof(exchangeRateVal)/sizeof(exchangeRateVal[0]); i++) {
        double randomValue = getRandomValue(RANDOM_INTERVAL);

        exchangeRateVal[i] = (exchangeRateVal[i] + randomValue > 0) ? (exchangeRateVal[i] + randomValue) : 0;
    }
    std::string str = " Dollar rate : " + std::to_string(exchangeRateVal[0]) +
        "\n Euro rate : " + std::to_string(exchangeRateVal[1]) +
        "\n Zloty rate : " + std::to_string(exchangeRateVal[2]);

    writeToFile("Rate.txt", str);
    return str;
}

DWORD WINAPI manageClientSubscription(LPVOID param) {
    SOCKET clientSocket = *((SOCKET*)(param)); // Comment in memory of interpret_cast

    while (1) {
        short int subscriptionMask = 0;

        // Get data from client
        if (recv(clientSocket, (char *)&subscriptionMask, sizeof(short int), 0) == SOCKET_ERROR) {
            std::cerr << " Receive failed (Lost connection to client). Error code: " << WSAGetLastError() << "\n";
            WaitForSingleObject(mutex, INFINITE);
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if disconnected
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << __TIME__ << " Deleted disconnected client" + std::to_string(i) + " from subscription list\n";
                }
            }
            ReleaseMutex(mutex);
            closesocket(clientSocket);
            return -1;
        }
        std::cout << " " << __TIME__ << " Received message from client : " << std::bitset<3>(subscriptionMask) << "\n";
        clientData client{};
        client.socket = clientSocket;
        client.subscription = subscriptionMask;
        int present = 0;

        WaitForSingleObject(mutex, INFINITE);
        if (subscriptionMask == 0) {
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if unsubscribed
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << __TIME__ << " Deleted unsubscribed client" + std::to_string(i) + " from list\n";
                }
            }
        }
        else {
            for (int i = 0; i < clients.size(); i++) {   // Add/update subscription
                if (clientSocket == clients[i].socket) {
                    clients[i].subscription = client.subscription;
                    present = 1;
                    std::cout << " " << __TIME__ << " Updated client" + std::to_string(i) + " subscription to " << std::bitset<3>(subscriptionMask) << "\n";
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
        mailingData packet;
        packet.type = 0b100;
        packet.message = weatherForecast;
        for (int i = 0; i < clients.size(); i++) {
            if ((clients[i].subscription & 0b100) == 0b100) {
                if (send(clients[i].socket, (char *)&packet, sizeof(mailingData), 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending weather forecast to client " << i << ". Error code: " << WSAGetLastError() << "\n";
                }
            }
        }
        ReleaseMutex(mutex);
        Sleep(20000);
       /* Sleep(60 * 60 * 1000);*/
    }
    return 0;
}
DWORD WINAPI sendExchangeRate(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        exchangeRate = randomExchangeRate();
        mailingData packet;
        packet.type = 0b010;
        packet.message = exchangeRate;
        for (int i = 0; i < clients.size(); i++) {
            if ((clients[i].subscription & 0b010) == 0b010) {
                if (send(clients[i].socket, (char *)&packet, sizeof(mailingData), 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending exchange rate to client " << i << ". Error code: " << WSAGetLastError() << "\n";
                    continue;
                }
            }
        }
        ReleaseMutex(mutex);
        Sleep(10000);
        /*Sleep(60 * 1000);*/
    }
    return 0;
}
DWORD WINAPI sendSharePrice(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        sharesRate = randomSharePrice();
        mailingData packet;
        packet.type = 0b001;
        packet.message = sharesRate;
        for (int i = 0; i < clients.size(); i++) {
            if ((clients[i].subscription & 0b001) == 0b001) {
                if (send(clients[i].socket, (char*)&packet, sizeof(mailingData), 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending shares rate to client " << i << ". Error code: " << WSAGetLastError() << "\n";
                    continue;
                }
            }
        }
        ReleaseMutex(mutex);
        Sleep(2500);
        /*Sleep(24 * 60 * 60 * 1000);*/
    }
    return 0;
}