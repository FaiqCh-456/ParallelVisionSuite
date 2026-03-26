#include "video_loader.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

struct VideoLoader::Impl {
    cv::VideoCapture cap;
    VideoInfo        info;
};

VideoLoader::VideoLoader(const std::string& path) {
    m_impl = new Impl();
    m_impl->cap.open(path);
    if (!m_impl->cap.isOpened()) {
        throw std::runtime_error("VideoLoader: cannot open '" + path + "'");
    }
    auto& info = m_impl->info;
    info.width      = (int)m_impl->cap.get(cv::CAP_PROP_FRAME_WIDTH);
    info.height     = (int)m_impl->cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    info.fps        = m_impl->cap.get(cv::CAP_PROP_FPS);
    info.frameCount = (int)m_impl->cap.get(cv::CAP_PROP_FRAME_COUNT);
    info.durationSec = (info.fps > 0)
                       ? info.frameCount / info.fps : 0.0;
}

VideoLoader::~VideoLoader() {
    if (m_impl) {
        m_impl->cap.release();
        delete m_impl;
    }
}

bool VideoLoader::isOpen() const {
    return m_impl && m_impl->cap.isOpened();
}

VideoLoader::VideoInfo VideoLoader::info() const {
    return m_impl ? m_impl->info : VideoInfo{};
}

bool VideoLoader::nextFrame(ImageBuffer& out) {
    cv::Mat frame;
    if (!m_impl->cap.read(frame)) return false;
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    out.width    = frame.cols;
    out.height   = frame.rows;
    out.channels = 3;
    out.data.assign(frame.data, frame.data + frame.total() * frame.elemSize());
    return true;
}

void VideoLoader::seekFrame(int frameIdx) {
    m_impl->cap.set(cv::CAP_PROP_POS_FRAMES, frameIdx);
}

std::vector<ImageBuffer> VideoLoader::extractAll() {
    std::vector<ImageBuffer> frames;
    frames.reserve(m_impl->info.frameCount);
    seekFrame(0);
    ImageBuffer buf;
    while (nextFrame(buf)) frames.push_back(buf);
    return frames;
}

void VideoLoader::streamFrames(
    std::function<void(int idx, ImageBuffer&)> cb)
{
    seekFrame(0);
    ImageBuffer buf;
    int idx = 0;
    while (nextFrame(buf)) {
        cb(idx++, buf);
    }
}

bool VideoLoader::writeVideo(const std::string& path,
                             const std::vector<ImageBuffer>& frames,
                             double fps)
{
    if (frames.empty()) return false;
    int fourcc = cv::VideoWriter::fourcc('m','p','4','v');
    cv::VideoWriter writer(path, fourcc, fps,
                           cv::Size(frames[0].width, frames[0].height));
    if (!writer.isOpened()) return false;
    for (const auto& f : frames) {
        cv::Mat mat(f.height, f.width, CV_8UC3,
                    const_cast<uint8_t*>(f.data.data()));
        cv::Mat bgr;
        cv::cvtColor(mat, bgr, cv::COLOR_RGB2BGR);
        writer.write(bgr);
    }
    writer.release();
    return true;
}