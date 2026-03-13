#pragma once
#include "image_buffer.h"
#include "filter_engine.h"
#include "video_loader.h"
#include <string>
#include <atomic>
#include <memory>

class Orchestrator {
public:
    Orchestrator();
    ~Orchestrator();

    // ── Initialisation ─────────────────────────────────────────────
    bool initGPU();
    bool gpuAvailable() const;
    std::string gpuName() const;
    FilterType currentFilter() const { return m_filter; }


    // ── File I/O dialogs (simple path input, no native dialog) ─────
    void openImageDialog();
    void openVideoDialog();
    void saveResultDialog();

    // ── Image state ───────────────────────────────────────────────
    bool             hasOriginalImage()  const;
    bool             hasProcessedImage() const;
    const ImageBuffer& originalImage()   const;
    const ImageBuffer& processedImage()  const;

    void loadImage(const std::string& path);

    // ── Video state ───────────────────────────────────────────────
    bool             hasVideo()     const;
    VideoLoader::VideoInfo videoInfo() const;
    ImageBuffer      getVideoFrame(int idx);
    void             loadVideo(const std::string& path);

    // ── Processing ────────────────────────────────────────────────
    void setFilter(FilterType ft);
    void setMode(ExecutionMode mode);
    void setNumThreads(int n);

    void processCurrentImage();
    void processCurrentVideo();

    bool  isProcessing()        const;
    float processingProgress()  const;

    double lastTimeMs()         const;
    double lastSerialTimeMs()   const;
    int    numThreads()         const;

private:
    ImageBuffer  m_original;
    ImageBuffer  m_processed;

    std::unique_ptr<VideoLoader> m_video;

    FilterType    m_filter    = FilterType::GAUSSIAN;
    ExecutionMode m_mode      = ExecutionMode::SERIAL;
    int           m_threads   = 4;

    std::atomic<bool>  m_processing{false};
    std::atomic<float> m_progress{0.0f};
    double             m_lastTimeMs       = 0.0;
    double             m_lastSerialTimeMs = 0.0;

    // Dialog state
    char m_imagePath[512] = {};
    char m_videoPath[512] = {};
    char m_savePath[512]  = {};
    bool m_showImgDialog  = false;
    bool m_showVidDialog  = false;
    bool m_showSaveDialog = false;

    void runDialogs();

    friend class GUI;   // GUI calls runDialogs() each frame
};