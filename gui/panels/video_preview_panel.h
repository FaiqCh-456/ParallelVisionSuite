<<<<<<< HEAD
#pragma once
#include "orchestrator.h"
#include "gl_texture.h"

class VideoPreviewPanel {
public:
    bool visible = true;
    explicit VideoPreviewPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    GLTexture     m_texFrame;
    bool          m_playing      = false;
    int           m_currentFrame = 0;
    double        m_lastFrameTime= 0.0;
=======
#pragma once
#include "orchestrator.h"
#include "gl_texture.h"

class VideoPreviewPanel {
public:
    bool visible = true;
    explicit VideoPreviewPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    GLTexture     m_texFrame;
    bool          m_playing      = false;
    int           m_currentFrame = 0;
    double        m_lastFrameTime= 0.0;
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};