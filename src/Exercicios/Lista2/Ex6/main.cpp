#include "OpenGLUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace
{
const char *vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;
uniform mat4 projection;

out vec3 interpolatedColor;

void main()
{
    gl_Position = projection * vec4(position, 1.0);
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

struct Vertex
{
    float x;
    float y;
    float z;
    float red;
    float green;
    float blue;
};

struct Point
{
    float x;
    float y;
};

struct AppState
{
    std::vector<Vertex> triangleVertices;
    std::vector<Point> pendingPoints;
    bool bufferDirty = true;
};

const std::array<std::array<float, 3>, 12> triangleColors = {{
    {0.92f, 0.18f, 0.70f},
    {0.12f, 0.76f, 0.82f},
    {0.96f, 0.63f, 0.12f},
    {0.23f, 0.82f, 0.46f},
    {0.50f, 0.36f, 0.94f},
    {0.94f, 0.31f, 0.28f},
    {0.35f, 0.65f, 0.98f},
    {0.78f, 0.82f, 0.18f},
    {0.98f, 0.45f, 0.70f},
    {0.20f, 0.88f, 0.70f},
    {0.72f, 0.42f, 0.20f},
    {0.70f, 0.70f, 0.95f}
}};

void updateTitle(GLFWwindow *window, const AppState &state)
{
    const std::size_t triangleCount = state.triangleVertices.size() / 3;
    const std::string title = "Lista 2 - Exercicio 6 | Triangulos: "
        + std::to_string(triangleCount)
        + " | Cliques pendentes: " + std::to_string(state.pendingPoints.size());
    glfwSetWindowTitle(window, title.c_str());
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    AppState *state = static_cast<AppState *>(glfwGetWindowUserPointer(window));
    if (!state) return;

    double cursorX = 0.0;
    double cursorY = 0.0;
    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    if (windowWidth <= 0 || windowHeight <= 0) return;

    // Converte a posição atual da janela para o mundo fixo de 800 x 600.
    const Point point = {
        static_cast<float>(cursorX * 800.0 / windowWidth),
        static_cast<float>(cursorY * 600.0 / windowHeight)
    };
    state->pendingPoints.push_back(point);

    if (state->pendingPoints.size() == 3)
    {
        const std::size_t triangleIndex = state->triangleVertices.size() / 3;
        const std::array<float, 3> &color =
            triangleColors[triangleIndex % triangleColors.size()];
        for (const Point &pending : state->pendingPoints)
        {
            state->triangleVertices.push_back({
                pending.x, pending.y, 0.0f, color[0], color[1], color[2]
            });
        }
        state->pendingPoints.clear();
        std::cout << "Triangulo " << triangleIndex + 1 << " criado.\n";
    }

    state->bufferDirty = true;
    updateTitle(window, *state);
}

void keyCallback(GLFWwindow *window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    if (key == GLFW_KEY_C)
    {
        AppState *state = static_cast<AppState *>(glfwGetWindowUserPointer(window));
        if (!state) return;
        state->triangleVertices.clear();
        state->pendingPoints.clear();
        state->bufferDirty = true;
        updateTitle(window, *state);
        std::cout << "Desenho limpo.\n";
    }
}

void updateBuffer(GLuint vbo, const AppState &state, std::vector<Vertex> &drawVertices)
{
    drawVertices = state.triangleVertices;
    for (const Point &point : state.pendingPoints)
        drawVertices.push_back({point.x, point.y, 0.0f, 1.0f, 1.0f, 1.0f});

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(drawVertices.size() * sizeof(Vertex)),
                 drawVertices.empty() ? nullptr : drawVertices.data(),
                 GL_DYNAMIC_DRAW);
}
}

int main()
{
    GLFWwindow *window = createWindow("Lista 2 - Exercicio 6", 800, 600, false);
    if (!window) return -1;

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, red)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    const glm::mat4 projection = glm::ortho(
        0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    AppState state;
    std::vector<Vertex> drawVertices;
    glfwSetWindowUserPointer(window, &state);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    updateTitle(window, state);
    glPointSize(12.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (state.bufferDirty)
        {
            updateBuffer(vbo, state, drawVertices);
            state.bufferDirty = false;
        }

        glClearColor(0.06f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);

        const GLsizei triangleVertexCount =
            static_cast<GLsizei>(state.triangleVertices.size());
        const GLsizei pendingVertexCount =
            static_cast<GLsizei>(state.pendingPoints.size());
        glDrawArrays(GL_TRIANGLES, 0, triangleVertexCount);
        glDrawArrays(GL_POINTS, triangleVertexCount, pendingVertexCount);
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
