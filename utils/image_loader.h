#pragma once
#include "image_buffer.h"
#include <string>

class ImageLoader {
public:
    // Load image from disk → ImageBuffer (RGB or RGBA)
    static ImageBuffer load(const std::string& path, int forceChannels = 3);

    // Save ImageBuffer to disk (PNG)
    static bool save(const ImageBuffer& buf, const std::string& path);

    // Create a synthetic test image (gradient) for benchmarks
    static ImageBuffer createTestImage(int width, int height, int channels = 3);
};