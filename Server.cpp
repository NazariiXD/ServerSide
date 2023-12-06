#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "utilityFunctions.h"

const int PORT = 12345;
const char* SERVER_IP = "127.0.0.1";
HANDLE mutex;

int main() {
	// Sockets library initialisation
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock!\n";
        return -1;
    }
    sockaddr_in serverAddress{};
    // Use inet_pton to convert an IP address from a string representation
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) != 1) {
        std::cerr << "Invalid IP address\n";
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
        std::cerr << "Error creating socket. Error code : " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }
    // Binding for accepting connections
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error code : " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        std::cerr << "Failed to create mutex\n";
    }
    if (CreateThread(NULL, NULL, sendWeatherForecast, NULL, NULL, NULL) == NULL) {
        std::cerr << "Failed to launch forecast subscription service\n";
    }
    if (CreateThread(NULL, NULL, sendExchangeRate, NULL, NULL, NULL) == NULL) {
        std::cerr << "Failed to launch exchange rate subscription service\n";
    }
    if (CreateThread(NULL, NULL, sendSharePrice, NULL, NULL, NULL) == NULL) {
        std::cerr << "Failed to launch shares rate subscription service\n";
    }
    std::cout << "Database connected\n";

    // Waiting for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server listening on port: " << PORT << "...\n";

    // Creating client socket
    SOCKET clientSocket;
    //client address
    sockaddr_in clientAddress{};
    //size of client address
    int client_addr_size = sizeof(clientAddress);
    while (1) {

        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &client_addr_size);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed. Error code : " << WSAGetLastError() << "\n";
            /*closesocket(serverSocket);
            WSACleanup();
            return -1;*/
            continue;
        }

        std::cout << " " << __TIME__ << " Client connected\n";

        //Processing of client requests
        CreateThread(NULL, 0, manageClientSubscription, &clientSocket, 0, NULL);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

