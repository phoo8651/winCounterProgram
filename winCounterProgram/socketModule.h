#ifndef SOCKETMODULE_H
#define SOCKETMODULE_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace Socket {
    // Winsock 라이브러리 링크
#pragma comment(lib, "ws2_32.lib")

    class Module {
    public:
        Module();
        ~Module();

        // 윈속 초기화 및 클리닝
        int initializeWinsock();
        void cleanupWinsock();

        // 소켓 생성
        SOCKET createSocket();

        // 서버 소켓 바인딩 및 리스닝
        int bindAndListenSocket(SOCKET socket, int port);

        // 클라이언트 소켓 연결
        int connectToServer(SOCKET socket, const char* serverIP, int port);

        // 데이터 송신
        int sendMessage(SOCKET socket, const std::string message);

        // 데이터 수신
        int receiveMessage(SOCKET socket, char* buffer, int bufferLength);

        // 소켓 종료
        void closeSocket(SOCKET socket);
    };
}

#endif // SOCKETMODULE_H
