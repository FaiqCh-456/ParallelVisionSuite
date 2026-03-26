#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Central data structure passed between all modules
struct ImageBuffer {
    int width      = 0;
    int height     = 0;
    int channels   = 0;   // 1=grey, 3=RGB, 4=RGBA
    std::vector<uint8_t> data;

    bool empty() const { return data.empty(); }

    size_t byteSize() const {
        return static_cast<size_t>(width) * height * channels;
    }

    // Pixel access helpers (row-major, packed RGB)
    uint8_t& at(int row, int col, int ch) {
        return data[(row * width + col) * channels + ch];
    }
    const uint8_t& at(int row, int col, int ch) const {
        return data[(row * width + col) * channels + ch];
    }

    // Clone
    ImageBuffer clone() const {
        ImageBuffer b;
        b.width    = width;
        b.height   = height;
        b.channels = channels;
        b.data     = data;
        return b;
    }

    std::string info() const {
        return std::to_string(width) + "x" + std::to_string(height)
             + "x" + std::to_string(channels);
    }
};