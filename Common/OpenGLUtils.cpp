#include "OpenGLUtils.h"

#include <algorithm>
#include <iostream>

namespace
{
void framebufferSizeCallback(GLFWwindow *, int width, int height)
{
    // Uma viewport quadrada evita que círculos e pixels sejam deformados.
    const int side = std::min(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void fullFramebufferSizeCallback(GLFWwindow *, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool checkShader(GLuint shader, const char *name)
{
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE)
        return true;

    GLchar log[1024];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    std::cerr << "Erro ao compilar " << name << ":\n" << log << '\n';
    return false;
}
}

GLFWwindow *createWindow(const char *title, int width, int height, bool squareViewport)
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cerr << "Falha ao inicializar GLFW.\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar a janela GLFW.\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Falha ao inicializar GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    GLFWframebuffersizefun viewportCallback = squareViewport
        ? framebufferSizeCallback
        : fullFramebufferSizeCallback;
    glfwSetFramebufferSizeCallback(window, viewportCallback);
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    viewportCallback(window, framebufferWidth, framebufferHeight);

    std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
    return window;
}

GLuint createShaderProgram(const char *vertexSource, const char *fragmentSource)
{
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    if (!checkShader(vertexShader, "vertex shader"))
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    if (!checkShader(fragmentShader, "fragment shader"))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        GLchar log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Erro ao ligar o programa de shaders:\n" << log << '\n';
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void processEscapeKey(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
