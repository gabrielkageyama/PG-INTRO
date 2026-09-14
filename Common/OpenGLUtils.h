#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow *createWindow(const char *title, int width = 800, int height = 600,
                         bool squareViewport = true);
GLuint createShaderProgram(const char *vertexSource, const char *fragmentSource);
void processEscapeKey(GLFWwindow *window);

#endif
