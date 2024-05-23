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

// Ctrl+C ���ͷ�Ʈ ó���� ���� ���� ����
volatile std::sig_atomic_t gSignalStatus = 0;

void signalHandler(int signal) {
    gSignalStatus = signal;
}

int handleManualMode(const std::string& fname);
int handleBackgroundMode();
int handleStartupMode();

int main() {
    std::string filename = "Settings.ini";

    // ������ �������� ������ �⺻ ���������� ���� ����
    std::ifstream inFile(filename);
    if (!inFile.good()) {
        createDefaultSettings(filename);
    }
    inFile.close();

    // �������� �ε�
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


// Mode�� ���� �Լ�
int handleManualMode(const std::string& fname) {
    std::cout << "Handling Manual Mode" << std::endl;

    // �Է� �ޱ�
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
        // ������ ���
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

    Socket::Module socketModule; // SocketModule ��ü ����
    RSAHandler rsaHandler;

    result = socketModule.initializeWinsock(); // SocketModule�� �ʱ�ȭ �Լ� ȣ��
    if (result != 0) {
        std::printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKET connectSocket = socketModule.createSocket(); // SocketModule�� ���� ���� �Լ� ȣ��
    if (connectSocket == INVALID_SOCKET) {
        socketModule.cleanupWinsock(); // SocketModule�� Ŭ���� �Լ� ȣ��
        return 1;
    }
    result = socketModule.connectToServer(connectSocket, RemoteAddr, RemotePort); // SocketModule�� ���� �Լ� ȣ��
    if (result != 0) {
        socketModule.closeSocket(connectSocket); // SocketModule�� ���� �ݱ� �Լ� ȣ��
        socketModule.cleanupWinsock(); // SocketModule�� Ŭ���� �Լ� ȣ��
        return 1;
    }

    SystemMonitor monitor;

    // Ctrl+C ���ͷ�Ʈ�� ó���� signal handler ���
    std::signal(SIGINT, signalHandler);

    while (!gSignalStatus) {
        // CPU ���� ��������
        double cpuUsage = monitor.GetCPULoad();

        // �޸� ���� ��������
        double memoryUsage = monitor.GetMemoryLoad();

        // ��ũ ���� ��������
        double diskUsage = monitor.GetDiskLoad();

        // ������ ����
        char data[1024];
        sprintf_s(data, "{\"name\":\"%s\",\"cpu_usage\":%.2f,\"memory_usage\":%.2f,\"disk_usage\":%.2f}", Hostname, cpuUsage, memoryUsage, diskUsage);
        std::cout << data << std::endl;

        try {
            // ���� Ű �б�
            rsaHandler.loadPublicKey("public_key.pem");

            // ������ ��ȣȭ
            std::string encryptedData = rsaHandler.encrypt(data);

            // ��ȣȭ�� �����͸� Base64�� ���ڵ� (������ ����)
            const std::string base64EncodedData = base64_encode(reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size());
            // ������ �۽�
            result = socketModule.sendMessage(connectSocket, base64EncodedData); // SocketModule�� ������ �۽� �Լ� ȣ��
            if (result == SOCKET_ERROR) {
                std::cout << "Failed to send data." << std::endl;
                break;
            }

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // 0.1�ʸ��� ����
        std::this_thread::sleep_for(std::chrono::seconds(Interval));
    }

    // ���� ����
    socketModule.closeSocket(connectSocket); // SocketModule�� ���� �ݱ� �Լ� ȣ��
    socketModule.cleanupWinsock(); // SocketModule�� Ŭ���� �Լ� ȣ��
    std::cout << "Exited manual mode gracefully." << std::endl;
    return 0;
}


int handleBackgroundMode() {
    std::cout << "Handling Background Mode" << std::endl;
    // Background ��忡 ���� ó�� �ڵ� �߰�

    return 0;
}

int handleStartupMode() {
    std::cout << "Handling Startup Mode" << std::endl;
    // Startup ��忡 ���� ó�� �ڵ� �߰�

    return 0;
}