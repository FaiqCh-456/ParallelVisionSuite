#include "asset_generator.h"
#include "image_loader.h"
#include "image_buffer.h"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

void AssetGenerator::generateIfMissing() {
    fs::create_directories("assets/images");

    struct Entry { int size; std::string path; };
    std::vector<Entry> targets = {
        { 512,  "assets/images/test_512.png"  },
        { 1024, "assets/images/test_1024.png" },
        { 2048, "assets/images/test_2048.png" },
    };

    for (auto& e : targets) {
        if (!fs::exists(e.path)) {
            generateTestImage(e.size, e.path);
        }
    }

    if (!fs::exists("assets/images/checkerboard.png"))
        generateCheckerboard(512, "assets/images/checkerboard.png");

    if (!fs::exists("assets/images/gradient.png"))
        generateGradient(512, "assets/images/gradient.png");
}

void AssetGenerator::generateTestImage(int size, const std::string& path) {
    ImageBuffer buf;
    buf.width    = size;
    buf.height   = size;
    buf.channels = 3;
    buf.data.resize((size_t)size * size * 3);

    float cx = size * 0.5f;
    float cy = size * 0.5f;

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            float dx   = c - cx;
            float dy   = r - cy;
            float dist = std::sqrt(dx*dx + dy*dy);
            float ring = std::sin(dist * 0.08f) * 0.5f + 0.5f;
            float diag = std::cos((r + c) * 0.04f) * 0.5f + 0.5f;
            float wave = std::sin(r * 0.06f) * std::cos(c * 0.06f)
                       * 0.5f + 0.5f;

            int idx = (r * size + c) * 3;
            buf.data[idx + 0] = (uint8_t)(ring * 255.0f);
            buf.data[idx + 1] = (uint8_t)(diag * 255.0f);
            buf.data[idx + 2] = (uint8_t)(wave * 255.0f);
        }
    }
    ImageLoader::save(buf, path);
}

void AssetGenerator::generateCheckerboard(int size, const std::string& path) {
    ImageBuffer buf;
    buf.width    = size;
    buf.height   = size;
    buf.channels = 3;
    buf.data.resize((size_t)size * size * 3);

    int cellSize = size / 16;
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            bool white = ((r / cellSize) + (c / cellSize)) % 2 == 0;
            uint8_t val = white ? 240 : 20;
            int idx = (r * size + c) * 3;
            buf.data[idx + 0] = val;
            buf.data[idx + 1] = val;
            buf.data[idx + 2] = val;
        }
    }
    ImageLoader::save(buf, path);
}

void AssetGenerator::generateGradient(int size, const std::string& path) {
    ImageBuffer buf;
    buf.width    = size;
    buf.height   = size;
    buf.channels = 3;
    buf.data.resize((size_t)size * size * 3);

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            float u = (float)c / size;
            float v = (float)r / size;
            int idx = (r * size + c) * 3;
            buf.data[idx + 0] = (uint8_t)(u * 255.0f);
            buf.data[idx + 1] = (uint8_t)(v * 255.0f);
            buf.data[idx + 2] = (uint8_t)((1.0f - u) * 255.0f);
        }
    }
    ImageLoader::save(buf, path);
}