#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer2D.h"
#include "RenderCheck.h"
#include "Game.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

namespace
{
constexpr int logicalWidth = 800;
constexpr int logicalHeight = 600;
const glm::vec3 background(0.40f, 0.64f, 0.72f);

void reportGlfwError(int code, const char *description)
{
    std::cerr << "GLFW " << code << ": " << description << '\n';
}

void resizeFramebuffer(GLFWwindow *, int width, int height)
{
    // Preserva a proporcao 4:3 ao redimensionar, sem deformar o sprite.
    const float scale = std::min(width / static_cast<float>(logicalWidth),
                                 height / static_cast<float>(logicalHeight));
    const int viewWidth = static_cast<int>(logicalWidth * scale);
    const int viewHeight = static_cast<int>(logicalHeight * scale);
    glViewport((width - viewWidth) / 2, (height - viewHeight) / 2, viewWidth, viewHeight);
}

void handleKey(GLFWwindow *window, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    auto *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
    if (game)
        game->handleKey(key, action);
}
}

int main(int argc, char **argv)
{
    glfwSetErrorCallback(reportGlfwError);
    if (glfwInit() != GLFW_TRUE)
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(logicalWidth, logicalHeight,
                                         "Trabalho Grau A - Corrida Medieval", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Falha ao inicializar GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLint major = 0, minor = 0, profile = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (major < 3 || (major == 3 && minor < 3) || !(profile & GL_CONTEXT_CORE_PROFILE_BIT))
    {
        std::cerr << "O jogo exige OpenGL 3.3 Core ou superior.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n'
              << "OpenGL: " << glGetString(GL_VERSION) << '\n'
              << "Perfil: Core\nAssets: " << TRABALHO_GRAU_A_ASSET_DIR << std::endl;

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, handleKey);
    glfwSetFramebufferSizeCallback(window, resizeFramebuffer);
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    resizeFramebuffer(window, width, height);

    int frames = 0;
    bool valid = true;
    {
        // Os destrutores liberam os recursos enquanto o contexto ainda existe.
        Renderer2D renderer;
        Game game;
        const std::string assets = TRABALHO_GRAU_A_ASSET_DIR;
        valid = renderer.initialize(logicalWidth, logicalHeight) && game.initialize(assets);
        glfwSetWindowUserPointer(window, &game);
        double previousTime = glfwGetTime();
        std::string previousTitle;
        while (valid && !glfwWindowShouldClose(window))
        {
            const double now = glfwGetTime();
            // Evita saltos apos arrastar/minimizar a janela ou uma pausa do processo.
            const double deltaTime = std::clamp(now - previousTime, 0.0, 0.05);
            previousTime = now;
            game.update(deltaTime);
            glfwPollEvents();
            glClearColor(background.r, background.g, background.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            game.draw(renderer);
            const std::string title = game.windowTitle();
            if (title != previousTitle)
            {
                glfwSetWindowTitle(window, title.c_str());
                previousTitle = title;
            }
            glfwSwapBuffers(window);
        }
        glfwSetWindowUserPointer(window, nullptr);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return valid ? 0 : 1;
}
