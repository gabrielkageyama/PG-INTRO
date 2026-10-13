#ifndef GRAU_A_RENDERER_2D_H
#define GRAU_A_RENDERER_2D_H

#include "Texture.h"
#include <glm/glm.hpp>

// Uma geometria compartilhada; cada desenho recebe sua propria model matrix.
class Renderer2D
{
public:
    Renderer2D() = default;
    ~Renderer2D();
    Renderer2D(const Renderer2D &) = delete;
    Renderer2D &operator=(const Renderer2D &) = delete;

    bool initialize(float width, float height);
    void draw(const Texture &texture, glm::vec2 position, glm::vec2 size,
              glm::vec2 uvOffset = {0.0f, 0.0f}, glm::vec2 uvScale = {1.0f, 1.0f},
              float angle = 0.0f) const;
    void drawSolid(glm::vec2 position, glm::vec2 size, glm::vec4 color) const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint program = 0;
    GLint modelLocation = -1;
    GLint uvOffsetLocation = -1;
    GLint uvScaleLocation = -1;
    GLint texturedLocation = -1;
    GLint colorLocation = -1;
    void drawGeometry(glm::vec2 position, glm::vec2 size, glm::vec2 uvOffset,
                       glm::vec2 uvScale, float angle) const;
};

#endif
