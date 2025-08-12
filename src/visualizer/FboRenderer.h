#pragma once

#include <QOpenGLFunctions_3_3_Core>

class FboRenderer : protected QOpenGLFunctions_3_3_Core {
public:
    FboRenderer();
    ~FboRenderer();

    bool init(int width, int height);
    void cleanup();

    void bind();
    void release();

    GLuint texture() const { return _fbo_texture; }

private:
    GLuint _fbo = 0;
    GLuint _fbo_texture = 0;
    GLuint _rbo = 0;
};
