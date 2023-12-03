#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#define TEMPERATURE -3

#define STOCK_PRICE_MICROSOFT 374.51
#define STOCK_PRICE_GOOGLE 131.86
#define STOCK_PRICE_APPLE 191.24

#define EXCHANGE_DOLLAR_RATE 38.3
#define EXCHANGE_EURO_RATE 40.5
#define EXCHANGE_ZLOTY_RATE 9.3

using namespace std;

const int PORT = 12345;


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
    double zlotyRate = EXCHANGE_ZLOTY_RATE + getRandomValue(RANDOM_INTERVAL/2);

    string str = " Dollar rate : " + to_string(dollarRate) +
        "\n Euro rate : " + to_string(euroRate) +
        "\n Zloty rate : " + to_string(zlotyRate);

    writeToFile("Rate.txt", str);

    return str;
}

DWORD WINAPI sendWeatherForecast(PVOID pvParam) {

}
DWORD WINAPI sendSharePrice(PVOID pvParam) {

}
DWORD WINAPI sendExchangeRate(PVOID pvParam) {

}

int main() {

	// Sockets library initialisation
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock!" << endl;
        return -1;
    }

    sockaddr_in serverAddress{};
    // Use inet_pton to convert an IP address from a string representation
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) != 1) {
        cerr << "Invalid IP address" << endl;
        WSACleanup();
        return -1;
    }
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_family = AF_INET;

    // Creating server socket
    // AF_INET - internet socket
    // SOCK_STREAM - stream socket (with creating a connection)
    // NULL - default TCP protocol
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return -1;
    }

    // Binding for accepting connections
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Waiting for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "Server listening on port: " << PORT << endl;

    // Creating client socket
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int client_addr_size = sizeof(clientAddress);

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &client_addr_size);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));


        // Receiving data from the client
        if (recv(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            cerr << "Receive failed" << endl;
            closesocket(clientSocket);
            continue;
        }

        cout << "Received message from client: " << buffer << endl;

        // Sending the response to the client
        const char* response = "Server received your message.";
        send(clientSocket, response, strlen(response), 0);


        // Close the client socket
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

