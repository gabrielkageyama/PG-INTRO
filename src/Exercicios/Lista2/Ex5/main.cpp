#include "OpenGLUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
const char *vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 1.0);
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

void drawScene(int x, int y, int width, int height, GLuint vao)
{
    glViewport(x, y, width, height);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
}

int main()
{
    GLFWwindow *window = createWindow("Lista 2 - Exercicio 5", 800, 600, false);
    if (!window) return -1;

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    const GLfloat vertices[] = {
        400.0f, 120.0f, 0.0f,
        220.0f, 480.0f, 0.0f,
        580.0f, 480.0f, 0.0f
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
    const glm::mat4 projection = glm::ortho(
        0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    const GLint colorLocation = glGetUniformLocation(shaderProgram, "inputColor");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(colorLocation, 0.23f, 0.82f, 0.46f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processEscapeKey(window);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.06f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const int leftWidth = width / 2;
        const int rightWidth = width - leftWidth;
        const int bottomHeight = height / 2;
        const int topHeight = height - bottomHeight;

        drawScene(0, bottomHeight, leftWidth, topHeight, vao);
        drawScene(leftWidth, bottomHeight, rightWidth, topHeight, vao);
        drawScene(0, 0, leftWidth, bottomHeight, vao);
        drawScene(leftWidth, 0, rightWidth, bottomHeight, vao);
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
