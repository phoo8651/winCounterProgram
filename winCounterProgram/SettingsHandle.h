#pragma once
#ifndef SETTINGS_HANDLE_H
#define SETTINGS_HANDLE_H

#include <string>
#include <map>

// �⺻�� ����
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

// ���� ���� ����
extern std::string g_remoteAddr;
extern int g_remotePort;
extern std::string g_hostName;
extern Mode g_mode;
extern int g_interval;

// ���ڿ��� Mode ���������� ��ȯ�ϴ� �Լ�
Mode stringToMode(const std::string& modeStr);

// �⺻ �������� ������ Settings.ini ���� ���� �Լ�
void createDefaultSettings(const std::string& filename);

// ini ������ �Ľ��ϴ� �Լ�
std::map<std::string, std::string> parseIniFile(const std::string& filename);

// �������� �������� �Լ�
void loadSettings(const std::string& filename);

// �������� �����ϴ� �Լ�
void writeSettings(const std::string& filename);
#endif // SETTINGS_HANDLE_H
