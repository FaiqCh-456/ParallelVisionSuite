<<<<<<< HEAD
#include "gl_texture.h"
extern "C" {
    #include <glad/glad.h>
}
#include <stdexcept>

GLTexture::GLTexture() {}

GLTexture::~GLTexture() { release(); }

void GLTexture::release() {
    if (m_texID) {
        glDeleteTextures(1, &m_texID);
        m_texID = 0;
    }
}

void GLTexture::upload(const ImageBuffer& buf) {
    if (buf.empty()) return;
    uploadRaw(buf.data.data(), buf.width, buf.height, buf.channels);
}

void GLTexture::uploadRaw(const uint8_t* data, int w, int h, int channels) {
    if (!m_texID) glGenTextures(1, &m_texID);

    glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum fmt = (channels == 4) ? GL_RGBA
               : (channels == 3) ? GL_RGB
               : GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h,
                 0, fmt, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_width  = w;
    m_height = h;
=======
#include "gl_texture.h"
extern "C" {
    #include <glad/glad.h>
}
#include <stdexcept>

GLTexture::GLTexture() {}

GLTexture::~GLTexture() { release(); }

void GLTexture::release() {
    if (m_texID) {
        glDeleteTextures(1, &m_texID);
        m_texID = 0;
    }
}

void GLTexture::upload(const ImageBuffer& buf) {
    if (buf.empty()) return;
    uploadRaw(buf.data.data(), buf.width, buf.height, buf.channels);
}

void GLTexture::uploadRaw(const uint8_t* data, int w, int h, int channels) {
    if (!m_texID) glGenTextures(1, &m_texID);

    glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum fmt = (channels == 4) ? GL_RGBA
               : (channels == 3) ? GL_RGB
               : GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h,
                 0, fmt, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_width  = w;
    m_height = h;
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
}