#pragma once
#include <windows.h>
#include <stdexcept>

struct MemoryInfo {
    unsigned long long totalMemory;
    unsigned long long freeMemory;
    unsigned long long usedMemory;
};

class MemoryUsage {
public:
    static MemoryInfo getMemoryInfo() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);

        if (GlobalMemoryStatusEx(&memInfo)) {
            MemoryInfo info;
            info.totalMemory = memInfo.ullTotalPhys / (1024 * 1024); // Convertir a MB
            info.freeMemory = memInfo.ullAvailPhys / (1024 * 1024);  // Convertir a MB
            info.usedMemory = info.totalMemory - info.freeMemory;    // Calcular memoria usada
            return info;
        }
    }
};