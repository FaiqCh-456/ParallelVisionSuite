#pragma once
#include "orchestrator.h"
#include "gl_texture.h"

class ImagePreviewPanel {
public:
    bool visible = true;
    explicit ImagePreviewPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    GLTexture     m_texOriginal;
    GLTexture     m_texProcessed;
    bool          m_sideBy = true;
    float         m_zoom   = 1.0f;
};