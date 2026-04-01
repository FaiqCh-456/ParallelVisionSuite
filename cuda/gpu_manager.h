#pragma once
#include "image_buffer.h"
#include <string>
#include <vector>

struct GPUDeviceInfo {
    std::string name;
    size_t      totalMemoryMB = 0;
    int         multiprocessorCount = 0;
    int         maxThreadsPerBlock  = 0;
    int         computeMajor = 0;
    int         computeMinor = 0;
};

class GPUManager {
public:
    static GPUManager& instance();

    // Detect GPU at runtime; returns false if no CUDA device found
    bool init();
    bool isAvailable() const { return m_available; }
    const GPUDeviceInfo& deviceInfo() const { return m_info; }

    // Allocate device buffer, upload, download, free
    uint8_t* deviceAlloc(size_t bytes);
    void     deviceFree(uint8_t* ptr);
    void     uploadToDevice(const uint8_t* host, uint8_t* device, size_t bytes);
    void     downloadToHost(const uint8_t* device, uint8_t* host, size_t bytes);

    // Synchronise all CUDA streams
    void sync();

    // Helper: round up to next multiple of blockSize
    static int divUp(int n, int block) { return (n + block - 1) / block; }

    std::string statusString() const;

private:
    GPUManager() = default;
    bool         m_available = false;
    GPUDeviceInfo m_info;
};