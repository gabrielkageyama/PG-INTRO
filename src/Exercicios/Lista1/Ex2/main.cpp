#include "OpenGLUtils.h"

#include <cmath>
#include <string>
#include <vector>

namespace
{
constexpr float PI = 3.14159265358979323846f;

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

struct Shape
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
    GLenum primitive = GL_TRIANGLE_FAN;
    float red = 1.0f;
    float green = 1.0f;
    float blue = 1.0f;
    const char *name = "";
};

float radians(float degrees)
{
    return degrees * PI / 180.0f;
}

void addPoint(std::vector<GLfloat> &vertices, float x, float y)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(0.0f);
}

// Cria um setor circular. Uma volta completa forma um círculo ou polígono.
std::vector<GLfloat> makeFan(int segments, float startDegrees = 0.0f,
                             float endDegrees = 360.0f, float radius = 0.70f)
{
    std::vector<GLfloat> vertices;
    addPoint(vertices, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i)
    {
        const float t = static_cast<float>(i) / segments;
        const float angle = radians(startDegrees + (endDegrees - startDegrees) * t);
        addPoint(vertices, radius * std::cos(angle), radius * std::sin(angle));
    }
    return vertices;
}

std::vector<GLfloat> makeStar(int tips, float outerRadius, float innerRadius)
{
    std::vector<GLfloat> vertices;
    addPoint(vertices, 0.0f, 0.0f);
    const int perimeterPoints = tips * 2;
    for (int i = 0; i <= perimeterPoints; ++i)
    {
        const float angle = PI / 2.0f + i * 2.0f * PI / perimeterPoints;
        const float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        addPoint(vertices, radius * std::cos(angle), radius * std::sin(angle));
    }
    return vertices;
}

std::vector<GLfloat> makeSpiral(int points, float turns, float maximumRadius)
{
    std::vector<GLfloat> vertices;
    for (int i = 0; i < points; ++i)
    {
        const float t = static_cast<float>(i) / (points - 1);
        const float angle = t * turns * 2.0f * PI;
        const float radius = 0.02f + maximumRadius * t;
        addPoint(vertices, radius * std::cos(angle), radius * std::sin(angle));
    }
    return vertices;
}

Shape uploadShape(const std::vector<GLfloat> &vertices, GLenum primitive,
                  const char *name, float red, float green, float blue)
{
    Shape shape;
    shape.vertexCount = static_cast<GLsizei>(vertices.size() / 3);
    shape.primitive = primitive;
    shape.name = name;
    shape.red = red;
    shape.green = green;
    shape.blue = blue;

    glGenVertexArrays(1, &shape.vao);
    glGenBuffers(1, &shape.vbo);
    glBindVertexArray(shape.vao);
    glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return shape;
}

int selectedShape = 0;

void keyCallback(GLFWwindow *window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_7)
        selectedShape = key - GLFW_KEY_1;
}
}

int main()
{
    GLFWwindow *window = createWindow("Exercicio 2 - Circulo | Teclas 1-7");
    if (!window) return -1;
    glfwSetKeyCallback(window, keyCallback);

    const GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0)
    {
        glfwTerminate();
        return -1;
    }

    std::vector<Shape> shapes;
    shapes.push_back(uploadShape(makeFan(96), GL_TRIANGLE_FAN, "Circulo", 0.20f, 0.65f, 1.00f));
    shapes.push_back(uploadShape(makeFan(8), GL_TRIANGLE_FAN, "Octogono", 0.30f, 0.85f, 0.55f));
    shapes.push_back(uploadShape(makeFan(5, 90.0f, 450.0f), GL_TRIANGLE_FAN, "Pentagono", 0.75f, 0.40f, 1.00f));
    shapes.push_back(uploadShape(makeFan(72, 35.0f, 325.0f), GL_TRIANGLE_FAN, "Pac-man", 1.00f, 0.85f, 0.10f));
    shapes.push_back(uploadShape(makeFan(32, 20.0f, 105.0f), GL_TRIANGLE_FAN, "Fatia de pizza", 1.00f, 0.42f, 0.18f));
    shapes.push_back(uploadShape(makeStar(5, 0.76f, 0.34f), GL_TRIANGLE_FAN, "Estrela", 1.00f, 0.30f, 0.58f));
    shapes.push_back(uploadShape(makeSpiral(360, 4.0f, 0.72f), GL_LINE_STRIP, "Espiral", 0.25f, 0.90f, 0.90f));

    glUseProgram(shaderProgram);
    const GLint colorLocation = glGetUniformLocation(shaderProgram, "inputColor");
    glLineWidth(4.0f);
    int previousSelection = -1;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (selectedShape != previousSelection)
        {
            previousSelection = selectedShape;
            const std::string title = std::string("Exercicio 2 - ") + shapes[selectedShape].name + " | Teclas 1-7";
            glfwSetWindowTitle(window, title.c_str());
        }

        glClearColor(0.06f, 0.07f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        const Shape &shape = shapes[selectedShape];
        glUniform4f(colorLocation, shape.red, shape.green, shape.blue, 1.0f);
        glBindVertexArray(shape.vao);
        glDrawArrays(shape.primitive, 0, shape.vertexCount);
        glfwSwapBuffers(window);
    }

    for (const Shape &shape : shapes)
    {
        glDeleteBuffers(1, &shape.vbo);
        glDeleteVertexArrays(1, &shape.vao);
    }
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
