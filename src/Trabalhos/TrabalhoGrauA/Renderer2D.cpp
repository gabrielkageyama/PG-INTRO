#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace
{
const char *vertexSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoordinates;
uniform mat4 projection;
uniform mat4 model;
uniform vec2 uvOffset;
uniform vec2 uvScale;
out vec2 uv;
void main()
{
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
    uv = uvOffset + textureCoordinates * uvScale;
}
)glsl";

const char *fragmentSource = R"glsl(
#version 330 core
in vec2 uv;
uniform sampler2D spriteTexture;
uniform bool textured;
uniform vec4 solidColor;
out vec4 color;
void main()
{
    color = textured ? texture(spriteTexture, uv) : solidColor;
}
)glsl";

GLuint compileShader(GLenum type, const char *source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Falha no " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader:\n" << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
}

Renderer2D::~Renderer2D()
{
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    if (program != 0)
        glDeleteProgram(program);
}

bool Renderer2D::initialize(float width, float height)
{
    if (program != 0 || width <= 0.0f || height <= 0.0f)
        return false;
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertex == 0)
        return false;
    const GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragment == 0)
    {
        glDeleteShader(vertex);
        return false;
    }
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        char log[2048] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Falha ao ligar shaders:\n" << log << '\n';
        return false;
    }

    // Quad unitario: posicao XY e UV, origem no canto superior esquerdo.
    const GLfloat vertices[] = {
        // x     y     u     v
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    const GLuint indices[] = {0, 1, 2, 2, 3, 0};
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                          reinterpret_cast<void *>(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // O EBO continua associado ao VAO.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(program);
    const glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
    modelLocation = glGetUniformLocation(program, "model");
    uvOffsetLocation = glGetUniformLocation(program, "uvOffset");
    uvScaleLocation = glGetUniformLocation(program, "uvScale");
    texturedLocation = glGetUniformLocation(program, "textured");
    colorLocation = glGetUniformLocation(program, "solidColor");
    glUniform1i(glGetUniformLocation(program, "spriteTexture"), 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

void Renderer2D::draw(const Texture &texture, glm::vec2 position, glm::vec2 size,
                      glm::vec2 uvOffset, glm::vec2 uvScale, float angle) const
{
    glUseProgram(program);
    glUniform1i(texturedLocation, GL_TRUE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id());
    drawGeometry(position, size, uvOffset, uvScale, angle);
}

void Renderer2D::drawSolid(glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
    glUseProgram(program);
    glUniform1i(texturedLocation, GL_FALSE);
    glUniform4f(colorLocation, color.r, color.g, color.b, color.a);
    drawGeometry(position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f);
}

void Renderer2D::drawGeometry(glm::vec2 position, glm::vec2 size,
                              glm::vec2 uvOffset, glm::vec2 uvScale, float angle) const
{
    glm::mat4 model(1.0f);
    // Rotacao em torno do centro; angle=0 preserva a posicao superior esquerda.
    model = glm::translate(model, glm::vec3(position + size * 0.5f, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
    glUseProgram(program);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform2f(uvOffsetLocation, uvOffset.x, uvOffset.y);
    glUniform2f(uvScaleLocation, uvScale.x, uvScale.y);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
