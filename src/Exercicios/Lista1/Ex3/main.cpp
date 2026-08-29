#include "OpenGLUtils.h"

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
}

int main()
{
    GLFWwindow *window = createWindow("Exercicio 3 - Cor por vertice");
    if (!window) return -1;

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    // Cada linha contém posição (x, y, z) e cor (r, g, b).
    const GLfloat vertices[] = {
         0.00f,  0.68f, 0.0f,  1.0f, 0.0f, 0.0f, // P1: vermelho
        -0.68f, -0.58f, 0.0f,  0.0f, 1.0f, 0.0f, // P2: verde
         0.68f, -0.38f, 0.0f,  0.0f, 0.0f, 1.0f  // P3: azul
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLsizei stride = 6 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processEscapeKey(window);
        glClearColor(0.06f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
