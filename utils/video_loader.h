#pragma once
#include "image_buffer.h"
#include <string>
#include <vector>
#include <functional>

class VideoLoader {
public:
    struct VideoInfo {
        int width    = 0;
        int height   = 0;
        double fps   = 0.0;
        int frameCount = 0;
        double durationSec = 0.0;
    };

    explicit VideoLoader(const std::string& path);
    ~VideoLoader();

    bool isOpen() const;
    VideoInfo info() const;

    // Read next frame → ImageBuffer (RGB).  Returns false at EOF.
    bool nextFrame(ImageBuffer& out);

    // Seek to frame index
    void seekFrame(int frameIdx);

    // Extract all frames (warning: memory intensive for long videos)
    std::vector<ImageBuffer> extractAll();

    // Stream frames one by one and call callback
    void streamFrames(std::function<void(int idx, ImageBuffer&)> cb);

    // Write processed frames to output file
    static bool writeVideo(const std::string& path,
                           const std::vector<ImageBuffer>& frames,
                           double fps);

private:
    struct Impl;
    Impl* m_impl = nullptr;
};