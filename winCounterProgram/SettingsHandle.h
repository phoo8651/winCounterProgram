#pragma once
#ifndef SETTINGS_HANDLE_H
#define SETTINGS_HANDLE_H

#include <string>
#include <map>

// 기본값 설정
const std::string DEFAULT_REMOTE_ADDR = "127.0.0.1";
const int DEFAULT_REMOTE_PORT = 5078;
const std::string DEFAULT_HOST_NAME = "SERVER";
const std::string DEFAULT_MODE = "Manual";
const int DEFAULT_INTERVAL = 1;

enum class Mode {
    Manual,
    Background,
    Startup,
    Unknown
};

// 전역 변수 선언
extern std::string g_remoteAddr;
extern int g_remotePort;
extern std::string g_hostName;
extern Mode g_mode;
extern int g_interval;

// 문자열을 Mode 열거형으로 변환하는 함수
Mode stringToMode(const std::string& modeStr);

// 기본 설정값을 포함한 Settings.ini 파일 생성 함수
void createDefaultSettings(const std::string& filename);

// ini 파일을 파싱하는 함수
std::map<std::string, std::string> parseIniFile(const std::string& filename);

// 설정값을 가져오는 함수
void loadSettings(const std::string& filename);

// 설정값을 저장하는 함수
void writeSettings(const std::string& filename);
#endif // SETTINGS_HANDLE_H
