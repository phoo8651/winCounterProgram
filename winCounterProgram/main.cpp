#pragma warning(disable:4996)
#include "SettingsHandle.h"
#include "socketModule.h"
#include "SystemMonitor.h"
#include "RSAHandler.h"
#include "base64.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <csignal>

using namespace std;

// Ctrl+C 인터럽트 처리를 위한 전역 변수
volatile std::sig_atomic_t gSignalStatus = 0;

void signalHandler(int signal) {
    gSignalStatus = signal;
}

int handleManualMode(const std::string& fname);
int handleBackgroundMode();
int handleStartupMode();

int main() {
    std::string filename = "Settings.ini";

    // 파일이 존재하지 않으면 기본 설정값으로 파일 생성
    std::ifstream inFile(filename);
    if (!inFile.good()) {
        createDefaultSettings(filename);
    }
    inFile.close();

    // 설정값을 로드
    loadSettings(filename);

    switch (g_mode) {
    case Mode::Manual:
        if (handleManualMode(filename) != 0) {
            std::cout << "Failed to handle Manual Mode" << std::endl;
            return 1;
        }
        else {
            std::cout << "Successfully handled Manual Mode" << std::endl;
        }
        break;
    case Mode::Background:
        if (handleBackgroundMode() != 0) {
            std::cout << "Failed to handle Background Mode" << std::endl;
        }
        else {
            std::cout << "Successfully handled Background Mode" << std::endl;
        }
        break;
    case Mode::Startup:
        if (handleStartupMode() != 0) {
            std::cout << "Failed to handle Startup Mode" << std::endl;
        }
        else {
            std::cout << "Successfully handled Startup Mode" << std::endl;
        }
        break;
    default:
        std::cout << "Unknown mode" << std::endl;
        break;
    }

    return 0;
}


// Mode에 따른 함수
int handleManualMode(const std::string& fname) {
    std::cout << "Handling Manual Mode" << std::endl;

    // 입력 받기
    char Hostname[256], RemoteAddr[256];
    int RemotePort, Interval;

    std::cout << "Reset settings? (y/n): ";
    char reset;
    std::cin >> reset;
    if (reset == 'y') {
        std::cout << "Enter name: ";
        std::cin >> Hostname;
        std::cout << "Enter Remote IP: ";
        std::cin >> RemoteAddr;
        std::cout << "Enter Remote port: ";
        std::cin >> RemotePort;
        std::cout << "Enter Interval: ";
        std::cin >> Interval;

        g_remoteAddr = RemoteAddr;
        g_remotePort = RemotePort;
        g_hostName = Hostname;
        g_mode = Mode::Manual;
        g_interval = Interval;

        writeSettings(fname);
        std::cout << "Settings saved successfully." << std::endl;
    }
    else {
        // 설정값 출력
        std::cout << "Name: " << g_hostName << std::endl;
        std::cout << "Remote IP: " << g_remoteAddr << std::endl;
        std::cout << "Remote Port: " << g_remotePort << std::endl;
        std::cout << "Interval: " << g_interval << std::endl;

        strcpy(Hostname, g_hostName.c_str());
        strcpy(RemoteAddr, g_remoteAddr.c_str());
        RemotePort = g_remotePort;
        Interval = g_interval;
    }

    int result;

    Socket::Module socketModule; // SocketModule 객체 생성
    RSAHandler rsaHandler;

    result = socketModule.initializeWinsock(); // SocketModule의 초기화 함수 호출
    if (result != 0) {
        std::printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKET connectSocket = socketModule.createSocket(); // SocketModule의 소켓 생성 함수 호출
    if (connectSocket == INVALID_SOCKET) {
        socketModule.cleanupWinsock(); // SocketModule의 클리닝 함수 호출
        return 1;
    }
    result = socketModule.connectToServer(connectSocket, RemoteAddr, RemotePort); // SocketModule의 연결 함수 호출
    if (result != 0) {
        socketModule.closeSocket(connectSocket); // SocketModule의 소켓 닫기 함수 호출
        socketModule.cleanupWinsock(); // SocketModule의 클리닝 함수 호출
        return 1;
    }

    SystemMonitor monitor;

    // Ctrl+C 인터럽트를 처리할 signal handler 등록
    std::signal(SIGINT, signalHandler);

    while (!gSignalStatus) {
        // CPU 사용률 가져오기
        double cpuUsage = monitor.GetCPULoad();

        // 메모리 사용률 가져오기
        double memoryUsage = monitor.GetMemoryLoad();

        // 디스크 사용률 가져오기
        double diskUsage = monitor.GetDiskLoad();

        // 데이터 생성
        char data[1024];
        sprintf_s(data, "{\"name\":\"%s\",\"cpu_usage\":%.2f,\"memory_usage\":%.2f,\"disk_usage\":%.2f}", Hostname, cpuUsage, memoryUsage, diskUsage);
        std::cout << data << std::endl;

        try {
            // 공개 키 읽기
            rsaHandler.loadPublicKey("public_key.pem");

            // 데이터 암호화
            std::string encryptedData = rsaHandler.encrypt(data);

            // 암호화된 데이터를 Base64로 인코딩 (전송을 위해)
            const std::string base64EncodedData = base64_encode(reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size());
            // 데이터 송신
            result = socketModule.sendMessage(connectSocket, base64EncodedData); // SocketModule의 데이터 송신 함수 호출
            if (result == SOCKET_ERROR) {
                std::cout << "Failed to send data." << std::endl;
                break;
            }

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // 0.1초마다 갱신
        std::this_thread::sleep_for(std::chrono::seconds(Interval));
    }

    // 소켓 종료
    socketModule.closeSocket(connectSocket); // SocketModule의 소켓 닫기 함수 호출
    socketModule.cleanupWinsock(); // SocketModule의 클리닝 함수 호출
    std::cout << "Exited manual mode gracefully." << std::endl;
    return 0;
}


int handleBackgroundMode() {
    std::cout << "Handling Background Mode" << std::endl;
    // Background 모드에 대한 처리 코드 추가

    return 0;
}

int handleStartupMode() {
    std::cout << "Handling Startup Mode" << std::endl;
    // Startup 모드에 대한 처리 코드 추가

    return 0;
}