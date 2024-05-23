#ifndef SOCKETMODULE_H
#define SOCKETMODULE_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace Socket {
    // Winsock ���̺귯�� ��ũ
#pragma comment(lib, "ws2_32.lib")

    class Module {
    public:
        Module();
        ~Module();

        // ���� �ʱ�ȭ �� Ŭ����
        int initializeWinsock();
        void cleanupWinsock();

        // ���� ����
        SOCKET createSocket();

        // ���� ���� ���ε� �� ������
        int bindAndListenSocket(SOCKET socket, int port);

        // Ŭ���̾�Ʈ ���� ����
        int connectToServer(SOCKET socket, const char* serverIP, int port);

        // ������ �۽�
        int sendMessage(SOCKET socket, const std::string message);

        // ������ ����
        int receiveMessage(SOCKET socket, char* buffer, int bufferLength);

        // ���� ����
        void closeSocket(SOCKET socket);
    };
}

#endif // SOCKETMODULE_H
