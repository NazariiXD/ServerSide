#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <Windows.h>
#include <vector>
#include <string>
#include <time.h>
#include "utilityFunctions.h"

using namespace std;

const int PORT = 12345;

HANDLE mutex;

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

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        cerr << "Failed to create mutex" << endl;
    }
    if (CreateThread(NULL, NULL, updateWeatherForecast, NULL, NULL, NULL) == NULL) {
        cerr << "Failed to launch data generator" << endl;
    }
    if (CreateThread(NULL, NULL, updateExchangeRate, NULL, NULL, NULL) == NULL) {
        cerr << "Failed to launch data generator" << endl;
    }
    if (CreateThread(NULL, NULL, updateSharesRate, NULL, NULL, NULL) == NULL) {
        cerr << "Failed to launch data generator" << endl;
    }
    cout << "Database connected\n";

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
    //client address
    sockaddr_in clientAddress{};
    //size of client address
    int client_addr_size = sizeof(clientAddress);

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &client_addr_size);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddress.sin_addr), ipAddress, INET_ADDRSTRLEN);

        cout << " " << __TIME__ "  [" << ipAddress << "]  ";

        //Processing of client requests
        CreateThread(NULL, 0, processClientRequests, &clientSocket, 0, NULL);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

