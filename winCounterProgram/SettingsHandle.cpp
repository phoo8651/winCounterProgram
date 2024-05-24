#include "SettingsHandle.h"
#include <fstream>
#include <stdexcept>

// 전역 변수 초기화
std::string g_remoteAddr = DEFAULT_REMOTE_ADDR;
int g_remotePort = DEFAULT_REMOTE_PORT;
std::string g_hostName = DEFAULT_HOST_NAME;
int g_interval = DEFAULT_INTERVAL;

// 기본 설정값을 포함한 Settings.ini 파일 생성 함수
void createDefaultSettings(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Unable to open file for writing: " + filename);
    file << "remote_addr=" << DEFAULT_REMOTE_ADDR << "\n";
    file << "remote_port=" << DEFAULT_REMOTE_PORT << "\n";
    file << "host_name=" << DEFAULT_HOST_NAME << "\n";
    file << "interval=" << DEFAULT_INTERVAL << "\n";
    file.close();
}

// ini 파일을 파싱하는 함수
std::map<std::string, std::string> parseIniFile(const std::string& filename) {
    std::map<std::string, std::string> settings;
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Unable to open file for reading: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        auto delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            settings[key] = value;
        }
    }

    file.close();
    return settings;
}

// 설정값을 가져오는 함수
void loadSettings(const std::string& filename) {
    auto settings = parseIniFile(filename);

    if (settings.find("remote_addr") != settings.end()) g_remoteAddr = settings["remote_addr"];
    if (settings.find("remote_port") != settings.end()) g_remotePort = std::stoi(settings["remote_port"]);
    if (settings.find("host_name") != settings.end()) g_hostName = settings["host_name"];
    if (settings.find("interval") != settings.end()) g_interval = std::stoi(settings["interval"]);
}

// 설정값을 ini 파일에 저장하는 함수
void writeSettings(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Unable to open file for writing: " + filename);

    file << "remote_addr=" << g_remoteAddr << "\n";
    file << "remote_port=" << g_remotePort << "\n";
    file << "host_name=" << g_hostName << "\n";
    file << "interval=" << g_interval << "\n";

    file.close();
}
