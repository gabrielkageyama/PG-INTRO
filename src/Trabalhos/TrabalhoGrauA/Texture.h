#ifndef GRAU_A_TEXTURE_H
#define GRAU_A_TEXTURE_H

#include <glad/glad.h>
#include <string>

// Dona da textura OpenGL. Deve ser destruida antes de encerrar o contexto.
class Texture
{
public:
    Texture() = default;
    ~Texture();
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    bool load(const std::string &path);
    GLuint id() const { return textureId; }
    int width() const { return imageWidth; }
    int height() const { return imageHeight; }

private:
    GLuint textureId = 0;
    int imageWidth = 0;
    int imageHeight = 0;
};

#endif
