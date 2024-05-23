#include "SocketModule.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace Socket;

Module::Module() {
    // 积己磊
}

Module::~Module() {
    // 家戈磊
}

int Module::initializeWinsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void Module::cleanupWinsock() {
    WSACleanup();
}

SOCKET Module::createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
    }
    return sock;
}

int Module::bindAndListenSocket(SOCKET socket, int port) {
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(port);

    if (bind(socket, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        return WSAGetLastError();
    }

    if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        return WSAGetLastError();
    }

    return 0; // 己傍
}

int Module::connectToServer(SOCKET socket, const char* serverIP, int port) {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    if (connect(socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        return WSAGetLastError();
    }

    return 0; // 己傍
}

int Module::sendMessage(SOCKET socket, const std::string message) {
    return send(socket, message.c_str(), message.length(), 0);
}

int Module::receiveMessage(SOCKET socket, char* buffer, int bufferLength) {
    int bytesReceived = recv(socket, buffer, bufferLength, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // Ensure null-terminated string
    }
    return bytesReceived;
}

void Module::closeSocket(SOCKET socket) {
    closesocket(socket);
}
