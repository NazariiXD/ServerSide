#include "utilityFunctions.h"

extern HANDLE mutex;
std::string sharesRate;
std::string exchangeRate;
std::string weatherForecast;

double sharesPriceVal[3] = {STOCK_PRICE_MICROSOFT, STOCK_PRICE_GOOGLE, STOCK_PRICE_APPLE};
double exchangeRateVal[3] = {EXCHANGE_DOLLAR_RATE, EXCHANGE_EURO_RATE, EXCHANGE_ZLOTY_RATE};
double temperature = TEMPERATURE;

std::vector<clientData> clients;

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    localtime_s(&tm_now, &time_t_now);

    std::ostringstream ss;
    ss << std::put_time(&tm_now, "%H:%M:%S");
    return ss.str();
}
void writeToFile(const std::string& fileName, const std::string& data) {
    std::ofstream file(fileName, std::ios_base::app);
    if (file.is_open()) {
        file << getCurrentTime();
        file << "\n" + data + "\n";
        file.close();
    }
}
double getRandomValue(double randomInterval) {
    int interval = static_cast<int>(randomInterval * 100.0);
    double sign = (rand() % 2 == 0) ? 1.0 : -1.0;
    return sign * (static_cast<double>(rand() % (interval)) / 100);
}

std::string randomWeatherForecast() {
    const double RANDOM_INTERVAL = 2.0;
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
    std::ostringstream ss;
    ss << " Temperature : " << std::fixed << std::setprecision(2) << temperature << " °C";

    std::string str = ss.str();
    writeToFile("Weather.txt", str);
    return str;
}
std::string randomSharePrice() {
    const double RANDOM_INTERVALS[] = { 3.0, 3.0, 3.0 };
    for (int i = 0; i < sizeof(sharesPriceVal)/sizeof(sharesPriceVal[0]); i++) {
        double randomValue = getRandomValue(RANDOM_INTERVALS[i]);
        sharesPriceVal[i] = max(sharesPriceVal[i] + randomValue, 0);
    }
    std::ostringstream ss;
    ss << " Shares Microsoft : " << std::fixed << std::setprecision(2) << sharesPriceVal[0] <<
        "\n Shares Google : " << std::fixed << std::setprecision(2) << sharesPriceVal[1] <<
        "\n Shares Apple : " << std::fixed << std::setprecision(2) << sharesPriceVal[2];

    std::string str = ss.str();
    writeToFile("Shares.txt", str);
    return str;
}
std::string randomExchangeRate() {
    const double RANDOM_INTERVALS[] = {1.5, 1.0, 0.5};
    for (int i = 0; i < sizeof(exchangeRateVal)/sizeof(exchangeRateVal[0]); i++) {
        double randomValue = getRandomValue(RANDOM_INTERVALS[i]);
        exchangeRateVal[i] = max(exchangeRateVal[i] + randomValue, 0);
    }
    std::ostringstream ss;
    ss << " Dollar rate : " << std::fixed << std::setprecision(2) << exchangeRateVal[0] <<
        "\n Euro rate : " << std::fixed << std::setprecision(2) << exchangeRateVal[1] <<
        "\n Zloty rate : " << std::fixed << std::setprecision(2) << exchangeRateVal[2];

    std::string str = ss.str();
    writeToFile("Rate.txt", str);
    return str;
}

DWORD WINAPI manageClientSubscription(LPVOID param) {
    SOCKET clientSocket = *((SOCKET*)(param)); // Comment in memory of interpret_cast
    while (1) {
        short int subscriptionMask = 0b000;
        // Get data from client
        if (recv(clientSocket, (char *)&subscriptionMask, sizeof(short int), 0) == SOCKET_ERROR) {
            std::cerr << " Receive failed (Lost connection to client). Error code: " << WSAGetLastError() << "\n";
            WaitForSingleObject(mutex, INFINITE);
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if disconnected
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << getCurrentTime() << " Deleted disconnected client" + std::to_string(i) + " from subscription list\n";
                }
            }
            ReleaseMutex(mutex);
            closesocket(clientSocket); 
            return -1;
        }
        std::cout << " " << getCurrentTime() << " Received message from client : " << std::bitset<3>(subscriptionMask) << "\n";
        clientData client{};
        client.socket = clientSocket;
        client.subscription = subscriptionMask;
        int present = 0;
        WaitForSingleObject(mutex, INFINITE);
        if (subscriptionMask == 0) {
            for (int i = 0; i < clients.size(); i++) { // Delete socket from list if unsubscribed
                if (clientSocket == clients[i].socket) {
                    clients.erase(clients.begin() + i);
                    std::cout << " " << getCurrentTime() << " Deleted unsubscribed client" + std::to_string(i) + " from list\n";
                }
            }
        }
        else {
            for (int i = 0; i < clients.size(); i++) {   // Add/update subscription
                if (clientSocket == clients[i].socket) {
                    if (clients[i].subscription != client.subscription) {
                        clients[i].subscription = client.subscription;
                        std::cout << " " << getCurrentTime() << " Updated client" + std::to_string(i) + " subscription to " << std::bitset<3>(client.subscription) << "\n";
                    }
                    present = 1;
                    break;
                }
            }
            if (present == 0) {
                clients.push_back(client);
                std::cout << " " << getCurrentTime() << " Added client with " << std::bitset<3>(subscriptionMask) << " subscription mask\n";
            }
        }
        ReleaseMutex(mutex);
    }
    closesocket(clientSocket);
    return 0;
}

DWORD WINAPI sendWeatherForecast(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        weatherForecast = randomWeatherForecast();
        mailingData packet{};
        packet.type = 0b100;
        strcpy_s(packet.message, weatherForecast.c_str());
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
        mailingData packet{};
        packet.type = 0b010;
        strcpy_s(packet.message, exchangeRate.c_str());
        for (int i = 0; i < clients.size(); i++) {
            if ((clients[i].subscription & 0b010) == 0b010) {
                if (send(clients[i].socket, (char *)&packet, sizeof(mailingData), 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending exchange rate to client " << i << ". Error code: " << WSAGetLastError() << "\n";
                    continue;
                }
            }
        }
        ReleaseMutex(mutex);
        Sleep(30000);
        /*Sleep(24 * 60 * 60 * 1000);*/
    }
    return 0;
}
DWORD WINAPI sendSharePrice(LPVOID param) {
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        sharesRate = randomSharePrice();
        mailingData packet{};
        packet.type = 0b001;
        strcpy_s(packet.message, sharesRate.c_str());
        for (int i = 0; i < clients.size(); i++) {
            if ((clients[i].subscription & 0b001) == 0b001) {
                if (send(clients[i].socket, (char*)&packet, sizeof(mailingData), 0) == SOCKET_ERROR) {
                    std::cerr << "Error sending shares rate to client " << i << ". Error code: " << WSAGetLastError() << "\n";
                    continue;
                }
            }
        }
        ReleaseMutex(mutex);
        Sleep(5000);
        /*Sleep(60 * 1000);*/
    }
    return 0;
}