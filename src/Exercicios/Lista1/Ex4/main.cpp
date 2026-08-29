#include "OpenGLUtils.h"

#include <array>
#include <string>
#include <vector>

namespace
{
const char *vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;

out vec3 interpolatedColor;

void main()
{
    gl_Position = vec4(position, 1.0);
    interpolatedColor = vertexColor;
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 330 core
in vec3 interpolatedColor;
out vec4 color;

void main()
{
    color = vec4(interpolatedColor, 1.0);
}
)glsl";

struct Mesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
    GLenum primitive = GL_TRIANGLES;
};

void addVertex(std::vector<GLfloat> &vertices, float x, float y,
               const std::array<float, 3> &color)
{
    vertices.insert(vertices.end(), {x, y, 0.0f, color[0], color[1], color[2]});
}

void addPixel(std::vector<GLfloat> &vertices, float left, float bottom, float size,
              const std::array<float, 3> &color)
{
    const float right = left + size;
    const float top = bottom + size;

    // Um quadrado é formado por dois triângulos.
    addVertex(vertices, left,  bottom, color);
    addVertex(vertices, right, bottom, color);
    addVertex(vertices, right, top,    color);
    addVertex(vertices, left,  bottom, color);
    addVertex(vertices, right, top,    color);
    addVertex(vertices, left,  top,    color);
}

Mesh uploadMesh(const std::vector<GLfloat> &vertices, GLenum primitive)
{
    Mesh mesh;
    mesh.vertexCount = static_cast<GLsizei>(vertices.size() / 6);
    mesh.primitive = primitive;
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return mesh;
}

std::vector<GLfloat> makeGrid(float left, float bottom, int columns, int rows, float cellSize)
{
    const std::array<float, 3> gridColor = {0.18f, 0.20f, 0.27f};
    std::vector<GLfloat> vertices;
    for (int column = 0; column <= columns; ++column)
    {
        const float x = left + column * cellSize;
        addVertex(vertices, x, bottom, gridColor);
        addVertex(vertices, x, bottom + rows * cellSize, gridColor);
    }
    for (int row = 0; row <= rows; ++row)
    {
        const float y = bottom + row * cellSize;
        addVertex(vertices, left, y, gridColor);
        addVertex(vertices, left + columns * cellSize, y, gridColor);
    }
    return vertices;
}
}

int main()
{
    GLFWwindow *window = createWindow("Exercicio 4 - Coracao em pixel art");
    if (!window) return -1;

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    // 0 = vazio, 1 = contorno, 2 = vermelho, 3 = brilho.
    const std::vector<std::string> drawing = {
        "001110011100",
        "012221122210",
        "122222223321",
        "122222222321",
        "122222222221",
        "012222222210",
        "001222222100",
        "000122221000",
        "000012210000",
        "000001100000"
    };

    const int rows = static_cast<int>(drawing.size());
    const int columns = static_cast<int>(drawing.front().size());
    const float cellSize = 0.12f;
    const float left = -columns * cellSize / 2.0f;
    const float bottom = -rows * cellSize / 2.0f;
    const std::array<float, 3> black = {0.04f, 0.04f, 0.06f};
    const std::array<float, 3> red = {0.88f, 0.08f, 0.16f};
    const std::array<float, 3> highlight = {1.00f, 0.78f, 0.80f};

    std::vector<GLfloat> heartVertices;
    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            const char pixel = drawing[row][column];
            if (pixel == '0') continue;
            const std::array<float, 3> &color =
                pixel == '1' ? black : (pixel == '2' ? red : highlight);

            // A primeira linha da matriz deve aparecer no topo da tela.
            const float x = left + column * cellSize;
            const float y = bottom + (rows - row - 1) * cellSize;
            addPixel(heartVertices, x, y, cellSize, color);
        }
    }

    const Mesh grid = uploadMesh(makeGrid(left, bottom, columns, rows, cellSize), GL_LINES);
    const Mesh heart = uploadMesh(heartVertices, GL_TRIANGLES);
    glUseProgram(shaderProgram);
    glLineWidth(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processEscapeKey(window);
        glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // A grade é desenhada primeiro; os pixels coloridos ficam sobre ela.
        glBindVertexArray(grid.vao);
        glDrawArrays(grid.primitive, 0, grid.vertexCount);
        glBindVertexArray(heart.vao);
        glDrawArrays(heart.primitive, 0, heart.vertexCount);
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &grid.vbo);
    glDeleteVertexArrays(1, &grid.vao);
    glDeleteBuffers(1, &heart.vbo);
    glDeleteVertexArrays(1, &heart.vao);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
