#pragma once
#include "image_buffer.h"
#include <cstdint>

// Manages a single OpenGL 2D texture for image preview
class GLTexture {
public:
    GLTexture();
    ~GLTexture();

    // Upload ImageBuffer pixels → GPU texture
    void upload(const ImageBuffer& buf);

    // Upload raw RGBA bytes directly
    void uploadRaw(const uint8_t* data, int w, int h, int channels);

    // Bind for ImGui::Image()
    uint64_t id() const { return static_cast<uint64_t>(m_texID); }

    int width()  const { return m_width; }
    int height() const { return m_height; }

    bool valid() const { return m_texID != 0; }
    void release();

private:
    unsigned int m_texID  = 0;
    int          m_width  = 0;
    int          m_height = 0;
};