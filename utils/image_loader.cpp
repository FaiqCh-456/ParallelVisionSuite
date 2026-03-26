#include "image_loader.h"
#include <stdexcept>
#include <cstring>
#include <cmath>

// stb_image — header-only, define once here
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

ImageBuffer ImageLoader::load(const std::string& path, int forceChannels) {
    int w, h, ch;
    uint8_t* raw = stbi_load(path.c_str(), &w, &h, &ch, forceChannels);
    if (!raw) {
        throw std::runtime_error("ImageLoader: failed to load '" + path
                                 + "': " + stbi_failure_reason());
    }

    int outCh = (forceChannels > 0) ? forceChannels : ch;
    ImageBuffer buf;
    buf.width    = w;
    buf.height   = h;
    buf.channels = outCh;
    buf.data.assign(raw, raw + (size_t)w * h * outCh);
    stbi_image_free(raw);
    return buf;
}

bool ImageLoader::save(const ImageBuffer& buf, const std::string& path) {
    if (buf.empty()) return false;
    int result = stbi_write_png(path.c_str(),
                                buf.width, buf.height, buf.channels,
                                buf.data.data(),
                                buf.width * buf.channels);
    return result != 0;
}

ImageBuffer ImageLoader::createTestImage(int width, int height, int channels) {
    ImageBuffer buf;
    buf.width    = width;
    buf.height   = height;
    buf.channels = channels;
    buf.data.resize((size_t)width * height * channels);

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            uint8_t val = static_cast<uint8_t>(
                255.0 * (std::sin(r * 0.05) * 0.5 + 0.5) *
                        (std::cos(c * 0.05) * 0.5 + 0.5));
            int idx = (r * width + c) * channels;
            for (int ch = 0; ch < channels; ++ch) {
                buf.data[idx + ch] = (channels == 1) ? val
                    : (ch == 0 ? val
                    : (ch == 1 ? (uint8_t)(255 - val)
                    :            (uint8_t)(val / 2)));
            }
        }
    }
    return buf;
}