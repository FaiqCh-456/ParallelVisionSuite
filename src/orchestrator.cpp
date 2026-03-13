#include "orchestrator.h"
#include "image_loader.h"
#include "filter_engine.h"
#include "performance_logger.h"
#include "gpu_manager.h"
#include "timer.h"
#include "imgui.h"
#include <thread>
#include <sstream>
#include <cstring>
#include <chrono>

Orchestrator::Orchestrator() {
    memset(m_imagePath, 0, sizeof(m_imagePath));
    memset(m_videoPath, 0, sizeof(m_videoPath));
    memset(m_savePath,  0, sizeof(m_savePath));
}

Orchestrator::~Orchestrator() {}

bool Orchestrator::initGPU() {
    return GPUManager::instance().init();
}

bool Orchestrator::gpuAvailable() const {
    return GPUManager::instance().isAvailable();
}

std::string Orchestrator::gpuName() const {
    return GPUManager::instance().deviceInfo().name;
}

// ── File dialogs ──────────────────────────────────────────────────────
// FIXED: only set flags here — never call OpenPopup inside menu bar context

void Orchestrator::openImageDialog()  { m_showImgDialog  = true; }
void Orchestrator::openVideoDialog()  { m_showVidDialog  = true; }
void Orchestrator::saveResultDialog() { m_showSaveDialog = true; }

void Orchestrator::runDialogs() {
    // Trigger popups at top-level context (NOT inside menu bar)
    if (m_showImgDialog)  { ImGui::OpenPopup("Open Image##dlg");  m_showImgDialog  = false; }
    if (m_showVidDialog)  { ImGui::OpenPopup("Open Video##dlg");  m_showVidDialog  = false; }
    if (m_showSaveDialog) { ImGui::OpenPopup("Save Result##dlg"); m_showSaveDialog = false; }

    // Centre all popups on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    // ── Open Image ────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Open Image##dlg", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter full path to image (.png or .jpg):");
        ImGui::SetNextItemWidth(420);
        ImGui::InputText("##imgpath", m_imagePath, sizeof(m_imagePath));
        ImGui::Spacing();
        if (ImGui::Button("Load", ImVec2(120, 0))) {
            try {
                loadImage(std::string(m_imagePath));
            } catch (const std::exception& e) {
                PerformanceLogger::instance().logMessage(
                    std::string("[ERROR] ") + e.what());
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::Spacing();
        ImGui::TextDisabled("Example: C:\\Users\\hp\\Pictures\\photo.jpg");
        ImGui::EndPopup();
    }

    // ── Open Video ────────────────────────────────────────────────────
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Open Video##dlg", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter full path to video (.mp4 or .avi):");
        ImGui::SetNextItemWidth(420);
        ImGui::InputText("##vidpath", m_videoPath, sizeof(m_videoPath));
        ImGui::Spacing();
        if (ImGui::Button("Load", ImVec2(120, 0))) {
            try {
                loadVideo(std::string(m_videoPath));
            } catch (const std::exception& e) {
                PerformanceLogger::instance().logMessage(
                    std::string("[ERROR] ") + e.what());
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::Spacing();
        ImGui::TextDisabled("Example: C:\\Users\\hp\\Videos\\clip.mp4");
        ImGui::EndPopup();
    }

    // ── Save Result ───────────────────────────────────────────────────
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Save Result##dlg", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Output filename (.png):");
        ImGui::SetNextItemWidth(420);
        ImGui::InputText("##savepath", m_savePath, sizeof(m_savePath));
        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (hasProcessedImage()) {
                bool ok = ImageLoader::save(m_processed, std::string(m_savePath));
                PerformanceLogger::instance().logMessage(
                    ok ? std::string("[OK] Saved → ") + m_savePath
                       : std::string("[ERROR] Failed to save: ") + m_savePath);
            } else {
                PerformanceLogger::instance().logMessage(
                    "[WARN] No processed image to save. Run Process Image first.");
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::Spacing();
        ImGui::TextDisabled("Example: output.png  or  C:\\Users\\hp\\result.png");
        ImGui::EndPopup();
    }
}

// ── Image state ───────────────────────────────────────────────────────

void Orchestrator::loadImage(const std::string& path) {
    m_original  = ImageLoader::load(path);
    m_processed = ImageBuffer{};
    PerformanceLogger::instance().logMessage(
        "[OK] Loaded: " + path + "  " + m_original.info());
}

bool Orchestrator::hasOriginalImage()  const { return !m_original.empty(); }
bool Orchestrator::hasProcessedImage() const { return !m_processed.empty(); }
const ImageBuffer& Orchestrator::originalImage()  const { return m_original;  }
const ImageBuffer& Orchestrator::processedImage() const { return m_processed; }

// ── Video state ───────────────────────────────────────────────────────

void Orchestrator::loadVideo(const std::string& path) {
    m_video = std::make_unique<VideoLoader>(path);
    PerformanceLogger::instance().logMessage("[OK] Video loaded: " + path);
}

bool Orchestrator::hasVideo() const {
    return m_video && m_video->isOpen();
}

VideoLoader::VideoInfo Orchestrator::videoInfo() const {
    return m_video ? m_video->info() : VideoLoader::VideoInfo{};
}

ImageBuffer Orchestrator::getVideoFrame(int idx) {
    if (!m_video) return {};
    m_video->seekFrame(idx);
    ImageBuffer buf;
    m_video->nextFrame(buf);
    return buf;
}

// ── Configuration ─────────────────────────────────────────────────────

void Orchestrator::setFilter(FilterType ft)    { m_filter  = ft; }
void Orchestrator::setMode(ExecutionMode mode)  { m_mode    = mode; }
void Orchestrator::setNumThreads(int n)         { m_threads = n; }
int    Orchestrator::numThreads()        const  { return m_threads; }
bool   Orchestrator::isProcessing()      const  { return m_processing; }
float  Orchestrator::processingProgress()const  { return m_progress; }
double Orchestrator::lastTimeMs()        const  { return m_lastTimeMs; }
double Orchestrator::lastSerialTimeMs()  const  { return m_lastSerialTimeMs; }

// ── Image processing ──────────────────────────────────────────────────

void Orchestrator::processCurrentImage() {
    if (!hasOriginalImage() || m_processing) return;
    m_processing = true;
    m_progress   = 0.0f;

    std::thread([this]() {
        try {
            // Measure serial baseline
            double serialMs = 0.0;
            FilterEngine::applyTimed(m_original, m_filter,
                                     ExecutionMode::SERIAL, 1, serialMs);
            m_lastSerialTimeMs = serialMs;

            // Run selected mode
            double t = 0.0;
            m_processed = FilterEngine::applyTimed(
                m_original, m_filter, m_mode, m_threads, t);
            m_lastTimeMs = t;

            // Record result
            BenchRecord rec;
            rec.filterName     = filterName(m_filter);
            rec.modeName       = modeName(m_mode);
            rec.imageWidth     = m_original.width;
            rec.imageHeight    = m_original.height;
            rec.numThreads     = m_threads;
            rec.serialTimeMs   = serialMs;
            rec.parallelTimeMs = t;
            rec.speedup        = PerformanceLogger::computeSpeedup(serialMs, t);
            rec.efficiency     = PerformanceLogger::computeEfficiency(
                                     rec.speedup, m_threads);
            rec.fps            = (t > 0) ? 1000.0 / t : 0.0;
            PerformanceLogger::instance().addRecord(rec);

            std::ostringstream oss;
            oss << "[OK] " << rec.filterName << " / " << rec.modeName
                << "  time=" << t << " ms"
                << "  speedup=" << rec.speedup << "x"
                << "  fps=" << rec.fps;
            PerformanceLogger::instance().logMessage(oss.str());

        } catch (const std::exception& e) {
            PerformanceLogger::instance().logMessage(
                std::string("[ERROR] Processing: ") + e.what());
        }
        m_progress   = 1.0f;
        m_processing = false;
    }).detach();
}

// ── Video processing ──────────────────────────────────────────────────

void Orchestrator::processCurrentVideo() {
    if (!hasVideo() || m_processing) return;
    m_processing = true;
    m_progress   = 0.0f;

    std::thread([this]() {
        try {
            VideoLoader::VideoInfo info = m_video->info();
            std::vector<ImageBuffer> output;
            output.reserve(info.frameCount);
            int done = 0;

            m_video->streamFrames([&](int /*idx*/, ImageBuffer& frame) {
                double t = 0.0;
                output.push_back(FilterEngine::applyTimed(
                    frame, m_filter, m_mode, m_threads, t));
                ++done;
                m_progress = (float)done / (float)(info.frameCount > 0 ? info.frameCount : 1);
            });

            VideoLoader::writeVideo("output_video.mp4", output, info.fps);
            PerformanceLogger::instance().logMessage(
                "[OK] Video saved → output_video.mp4");

        } catch (const std::exception& e) {
            PerformanceLogger::instance().logMessage(
                std::string("[ERROR] Video: ") + e.what());
        }
        m_progress   = 1.0f;
        m_processing = false;
    }).detach();
}