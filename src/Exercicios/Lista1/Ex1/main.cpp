#include "OpenGLUtils.h"

#include <iostream>
#include <string>

namespace
{
const char *vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position, 1.0);
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 330 core
uniform vec4 inputColor;
out vec4 color;

void main()
{
    color = inputColor;
}
)glsl";

enum DrawMode
{
    FILLED = 1,
    OUTLINE = 2,
    POINTS = 3,
    COMBINED = 4
};

DrawMode currentMode = COMBINED;

void keyCallback(GLFWwindow *window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_4)
        currentMode = static_cast<DrawMode>(key - GLFW_KEY_0);
}

void drawFilled(GLint colorLocation)
{
    glUniform4f(colorLocation, 0.80f, 0.18f, 0.38f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawOutline(GLint colorLocation)
{
    glUniform4f(colorLocation, 1.0f, 0.85f, 0.15f, 1.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 3);
    glDrawArrays(GL_LINE_LOOP, 3, 3);
}

void drawPoints(GLint colorLocation)
{
    glUniform4f(colorLocation, 0.15f, 0.85f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, 6);
}
}

int main()
{
    GLFWwindow *window = createWindow("Exercicio 1 - Combinado | Teclas 1-4");
    if (!window) return -1;
    glfwSetKeyCallback(window, keyCallback);

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    // Dois triângulos com as coordenadas mostradas no enunciado.
    const GLfloat vertices[] = {
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.0f,  0.0f, 0.0f,

         0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    const GLint colorLocation = glGetUniformLocation(shaderProgram, "inputColor");
    if (colorLocation < 0)
        std::cerr << "Uniform inputColor nao encontrada.\n";

    glLineWidth(5.0f);
    glPointSize(12.0f);
    DrawMode previousMode = static_cast<DrawMode>(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (currentMode != previousMode)
        {
            previousMode = currentMode;
            const char *names[] = {"", "Preenchido", "Contorno", "Pontos", "Combinado"};
            const std::string title = std::string("Exercicio 1 - ") + names[currentMode] + " | Teclas 1-4";
            glfwSetWindowTitle(window, title.c_str());
        }

        glClearColor(0.06f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);

        if (currentMode == FILLED || currentMode == COMBINED) drawFilled(colorLocation);
        if (currentMode == OUTLINE || currentMode == COMBINED) drawOutline(colorLocation);
        if (currentMode == POINTS || currentMode == COMBINED) drawPoints(colorLocation);

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
