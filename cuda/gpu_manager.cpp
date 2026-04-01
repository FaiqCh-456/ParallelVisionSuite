#include "gpu_manager.h"
#include <sstream>
#include <stdexcept>
#include <cstring>

#ifdef HAS_CUDA
#include <cuda_runtime.h>

#define CUDA_CHECK(call)                                              \
    do {                                                              \
        cudaError_t err = (call);                                     \
        if (err != cudaSuccess) {                                     \
            throw std::runtime_error(std::string("CUDA error: ")     \
                + cudaGetErrorString(err)                             \
                + " at " __FILE__ ":" + std::to_string(__LINE__));   \
        }                                                             \
    } while(0)
#endif

GPUManager& GPUManager::instance() {
    static GPUManager inst;
    return inst;
}

bool GPUManager::init() {
#ifdef HAS_CUDA
    int count = 0;
    cudaError_t err = cudaGetDeviceCount(&count);
    if (err != cudaSuccess || count == 0) {
        m_available = false;
        return false;
    }
    cudaSetDevice(0);
    cudaDeviceProp prop;
    CUDA_CHECK(cudaGetDeviceProperties(&prop, 0));

    m_info.name                 = prop.name;
    m_info.totalMemoryMB        = prop.totalGlobalMem / (1024 * 1024);
    m_info.multiprocessorCount  = prop.multiProcessorCount;
    m_info.maxThreadsPerBlock   = prop.maxThreadsPerBlock;
    m_info.computeMajor         = prop.major;
    m_info.computeMinor         = prop.minor;
    m_available = true;
    return true;
#else
    m_available = false;
    return false;
#endif
}

uint8_t* GPUManager::deviceAlloc(size_t bytes) {
#ifdef HAS_CUDA
    void* ptr = nullptr;
    CUDA_CHECK(cudaMalloc(&ptr, bytes));
    return reinterpret_cast<uint8_t*>(ptr);
#else
    (void)bytes;
    return nullptr;
#endif
}

void GPUManager::deviceFree(uint8_t* ptr) {
#ifdef HAS_CUDA
    if (ptr) cudaFree(ptr);
#else
    (void)ptr;
#endif
}

void GPUManager::uploadToDevice(const uint8_t* host,
                                uint8_t* device,
                                size_t bytes) {
#ifdef HAS_CUDA
    CUDA_CHECK(cudaMemcpy(device, host, bytes, cudaMemcpyHostToDevice));
#else
    (void)host; (void)device; (void)bytes;
#endif
}

void GPUManager::downloadToHost(const uint8_t* device,
                                uint8_t* host,
                                size_t bytes) {
#ifdef HAS_CUDA
    CUDA_CHECK(cudaMemcpy(host, device, bytes, cudaMemcpyDeviceToHost));
#else
    (void)device; (void)host; (void)bytes;
#endif
}

void GPUManager::sync() {
#ifdef HAS_CUDA
    CUDA_CHECK(cudaDeviceSynchronize());
#endif
}

std::string GPUManager::statusString() const {
    if (!m_available) return "No CUDA GPU detected";
    std::ostringstream oss;
    oss << m_info.name
        << " | " << m_info.totalMemoryMB << " MB"
        << " | " << m_info.multiprocessorCount << " SMs"
        << " | sm_" << m_info.computeMajor << m_info.computeMinor;
    return oss.str();
}