<<<<<<< HEAD
#include "video_preview_panel.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

VideoPreviewPanel::VideoPreviewPanel(Orchestrator& orch) : m_orch(orch) {}

void VideoPreviewPanel::render() {
    ImGui::Begin("Video Preview", &visible);

    // FIX: copy VideoInfo instead of taking reference to temporary
    VideoLoader::VideoInfo info = m_orch.videoInfo();

    if (!m_orch.hasVideo()) {
        ImGui::TextDisabled("No video loaded.");
        ImGui::TextDisabled("Use File > Open Video");
        ImGui::End();
        return;
    }

    // Auto-advance frame when playing
    if (m_playing) {
        double now = glfwGetTime();
        double interval = (info.fps > 0) ? 1.0 / info.fps : 1.0 / 30.0;
        if (now - m_lastFrameTime >= interval) {
            m_currentFrame = (m_currentFrame + 1) % info.frameCount;
            m_lastFrameTime = now;
        }
    }

    // Show current frame
    ImageBuffer frame = m_orch.getVideoFrame(m_currentFrame);
    if (!frame.empty()) m_texFrame.upload(frame);

    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.y -= 80;
    if (m_texFrame.valid() && avail.x > 0 && avail.y > 0) {
        float ratio = (float)m_texFrame.height() / (float)m_texFrame.width();
        float dw = avail.x;
        float dh = dw * ratio;
        if (dh > avail.y) { dh = avail.y; dw = dh / ratio; }
        ImGui::Image((ImTextureID)(uint64_t)m_texFrame.id(), ImVec2(dw, dh));
    }

    ImGui::Separator();
    ImGui::Text("Frame: %d / %d   FPS: %.1f",
                m_currentFrame, info.frameCount, info.fps);

    if (info.frameCount > 1)
        ImGui::SliderInt("##seek", &m_currentFrame, 0, info.frameCount - 1);

    if (ImGui::Button(m_playing ? " || " : "  >  ")) m_playing = !m_playing;
    ImGui::SameLine();
    if (ImGui::Button(" |< ")) { m_currentFrame = 0; m_playing = false; }
    ImGui::SameLine();
    if (ImGui::Button("Process Video")) m_orch.processCurrentVideo();

    ImGui::End();
=======
#include "video_preview_panel.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

VideoPreviewPanel::VideoPreviewPanel(Orchestrator& orch) : m_orch(orch) {}

void VideoPreviewPanel::render() {
    ImGui::Begin("Video Preview", &visible);

    // FIX: copy VideoInfo instead of taking reference to temporary
    VideoLoader::VideoInfo info = m_orch.videoInfo();

    if (!m_orch.hasVideo()) {
        ImGui::TextDisabled("No video loaded.");
        ImGui::TextDisabled("Use File > Open Video");
        ImGui::End();
        return;
    }

    // Auto-advance frame when playing
    if (m_playing) {
        double now = glfwGetTime();
        double interval = (info.fps > 0) ? 1.0 / info.fps : 1.0 / 30.0;
        if (now - m_lastFrameTime >= interval) {
            m_currentFrame = (m_currentFrame + 1) % info.frameCount;
            m_lastFrameTime = now;
        }
    }

    // Show current frame
    ImageBuffer frame = m_orch.getVideoFrame(m_currentFrame);
    if (!frame.empty()) m_texFrame.upload(frame);

    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.y -= 80;
    if (m_texFrame.valid() && avail.x > 0 && avail.y > 0) {
        float ratio = (float)m_texFrame.height() / (float)m_texFrame.width();
        float dw = avail.x;
        float dh = dw * ratio;
        if (dh > avail.y) { dh = avail.y; dw = dh / ratio; }
        ImGui::Image((ImTextureID)(uint64_t)m_texFrame.id(), ImVec2(dw, dh));
    }

    ImGui::Separator();
    ImGui::Text("Frame: %d / %d   FPS: %.1f",
                m_currentFrame, info.frameCount, info.fps);

    if (info.frameCount > 1)
        ImGui::SliderInt("##seek", &m_currentFrame, 0, info.frameCount - 1);

    if (ImGui::Button(m_playing ? " || " : "  >  ")) m_playing = !m_playing;
    ImGui::SameLine();
    if (ImGui::Button(" |< ")) { m_currentFrame = 0; m_playing = false; }
    ImGui::SameLine();
    if (ImGui::Button("Process Video")) m_orch.processCurrentVideo();

    ImGui::End();
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
}