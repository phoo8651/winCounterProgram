#include "SystemMonitor.h"
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")

double SystemMonitor::GetCPULoad() {
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PDH_FMT_COUNTERVALUE counterVal;

    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
    Sleep(100); // 데이터 수집을 위해 잠시 대기
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    return counterVal.doubleValue;
}

double SystemMonitor::GetMemoryLoad() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    GlobalMemoryStatusEx(&memStatus);

    return (memStatus.dwMemoryLoad);
}

double SystemMonitor::GetDiskLoad() {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

    double diskUsage = 100.0 * (1.0 - (double)totalNumberOfFreeBytes.QuadPart / (double)totalNumberOfBytes.QuadPart);
    return diskUsage;
}
