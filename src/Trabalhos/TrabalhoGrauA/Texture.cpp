#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::~Texture()
{
    if (textureId != 0)
        glDeleteTextures(1, &textureId);
}

bool Texture::load(const std::string &path)
{
    int width = 0, height = 0, sourceChannels = 0;
    // Origem superior esquerda tanto na imagem quanto nos UVs do quad.
    stbi_set_flip_vertically_on_load(false);
    unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &sourceChannels, STBI_rgb_alpha);
    if (!pixels)
    {
        std::cerr << "Falha ao carregar " << path << ": " << stbi_failure_reason() << '\n';
        return false;
    }

    if (textureId != 0)
        glDeleteTextures(1, &textureId);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // RGBA possui quatro bytes por pixel e satisfaz o alinhamento padrao de 4.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    imageWidth = width;
    imageHeight = height;
    std::cout << "Textura: " << path << " (" << width << 'x' << height
              << ", canais de origem: " << sourceChannels << ", upload RGBA)\n";
    return true;
}
