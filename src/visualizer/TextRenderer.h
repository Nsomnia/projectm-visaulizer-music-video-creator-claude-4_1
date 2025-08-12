#pragma once

#include <string>
#include <map>
#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>

struct Character {
    GLuint     TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint     Advance;
};

class TextRenderer : protected QOpenGLFunctions_3_3_Core {
public:
    TextRenderer();
    ~TextRenderer();

    bool init(const std::string& fontPath, int fontSize);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void setProjection(int width, int height);

private:
    void initShaders();
    void initBuffers();

    std::map<char, Character> m_characters;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_shaderProgramId = 0;
    glm::mat4 m_projection;
};
