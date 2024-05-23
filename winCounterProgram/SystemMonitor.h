#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

class SystemMonitor {
public:
    double GetCPULoad();
    double GetMemoryLoad();
    double GetDiskLoad();
};

#endif // SYSTEM_MONITOR_H
