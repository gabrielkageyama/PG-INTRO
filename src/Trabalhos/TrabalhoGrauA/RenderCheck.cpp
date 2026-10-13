#include "RenderCheck.h"
#include "Game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <stb_image.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

bool checkSpritePixels(const std::string &path, glm::vec2 position, glm::vec2 size,
                       glm::vec2 logicalSize, glm::vec3 background,
                       int frame, int frameCount, float angle, bool flipX)
{
    int width = 0, height = 0, channels = 0;
    unsigned char *source = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!source)
        return false;
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] <= 0 || viewport[3] <= 0)
    {
        stbi_image_free(source);
        return false;
    }
    std::vector<unsigned char> actual(static_cast<std::size_t>(viewport[2]) * viewport[3] * 4);
    glReadBuffer(GL_BACK);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3],
                 GL_RGBA, GL_UNSIGNED_BYTE, actual.data());

    int differences = 0;
    int visibleSamples = 0, transparentSamples = 0;
    const int frameWidth = width / frameCount;
    const float cosine = std::cos(angle), sine = std::sin(angle);
    for (int y = 0; y < viewport[3]; ++y)
    {
        // glReadPixels tem origem inferior esquerda; nosso mundo tem Y para baixo.
        const float worldY = (viewport[3] - y - 0.5f) * logicalSize.y / viewport[3];
        for (int x = 0; x < viewport[2]; ++x)
        {
            const float worldX = (x + 0.5f) * logicalSize.x / viewport[2];
            // Rotacao inversa: transforma o pixel de tela no espaco local do sprite.
            const float dx = worldX - position.x - size.x * 0.5f;
            const float dy = worldY - position.y - size.y * 0.5f;
            const float localX = cosine * dx + sine * dy + size.x * 0.5f;
            const float localY = -sine * dx + cosine * dy + size.y * 0.5f;
            const unsigned char *texel = nullptr;
            if (localX >= 0.0f && localX < size.x && localY >= 0.0f && localY < size.y)
            {
                const int frameX = std::min(frameWidth - 1,
                    static_cast<int>(localX / size.x * frameWidth));
                const int sx = frame * frameWidth + (flipX ? frameWidth - 1 - frameX : frameX);
                const int sy = std::min(height - 1, static_cast<int>(localY / size.y * height));
                texel = source + (sy * width + sx) * 4;
                if (texel[3] > 0) ++visibleSamples;
                if (texel[3] == 0) ++transparentSamples;
            }
            const float alpha = texel ? texel[3] / 255.0f : 0.0f;
            for (int c = 0; c < 3; ++c)
            {
                const int clear = static_cast<int>(std::lround(background[c] * 255.0f));
                const int expected = static_cast<int>(std::lround(
                    (texel ? texel[c] : 0) * alpha + clear * (1.0f - alpha)));
                const std::size_t index = (static_cast<std::size_t>(y) * viewport[2] + x) * 4 + c;
                // Tolerancia para arredondamento/dithering de buffers de 8 bits.
                if (std::abs(static_cast<int>(actual[index]) - expected) > 2)
                    ++differences;
            }
        }
    }
    bool sourceHasTransparency = false;
    for (int y = 0; y < height; ++y)
        for (int x = frame * frameWidth; x < (frame + 1) * frameWidth; ++x)
            if (source[(y * width + x) * 4 + 3] == 0) sourceHasTransparency = true;
    stbi_image_free(source);
    // Os icones da loja sao opacos; sprites com transparencia continuam exigindo sua verificacao.
    const bool valid = differences == 0 && visibleSamples > 0 &&
        (!sourceHasTransparency || transparentSamples > 0);
    std::cout << "Comparacao GPU/PNG: " << (valid ? "OK" : "FALHOU")
              << "; canais divergentes: " << differences
              << "; amostras visiveis/transparentes: " << visibleSamples << '/' << transparentSamples << '\n';
    return valid;
}

bool checkSpriteSheets(const Renderer2D &renderer, const std::string &assets,
                       glm::vec2 logicalSize, glm::vec3 background)
{
    struct Sample { const char *path; int frames; float angle; bool flipX = false; };
    const Sample samples[] = {
        {"/ui/heart-full-32x32.png", 1, 0.0f},
        {"/ui/heart-empty-32x32.png", 1, 0.0f},
        {"/characters/player/knight-run-front-7x32.png", 7, 0.0f},
        {"/effects/sword-slash-6x64.png", 6, glm::half_pi<float>()},
        {"/characters/enemies/basic-orc-7x32.png", 7, 0.0f, true},
        {"/characters/enemies/projectile-mage-7x32.png", 7, 0.0f, true},
        {"/characters/enemies/adjacent-rogue-7x32.png", 7, 0.0f, true},
        {"/obstacles/medieval-obstacles-6x32.png", 6, 0.0f},
        {"/projectiles/fireball-left-4x32.png", 4, 0.0f},
        {"/items/coin-spin-4x32.png", 4, 0.0f},
        {"/ui/shop-attack-range-64x64.png", 1, 0.0f},
        {"/ui/shop-extra-life-64x64.png", 1, 0.0f},
        {"/ui/shop-extra-gold-64x64.png", 1, 0.0f}
    };
    // Verifica a cor/alpha do aviso do ladino antes de voltar ao modo texturizado.
    glClearColor(background.r, background.g, background.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    const glm::vec4 warningColor(1.0f, 0.85f, 0.1f, 0.4f);
    renderer.drawSolid({160.0f, 220.0f}, {96.0f, 48.0f}, warningColor);
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    unsigned char pixel[4] = {};
    glReadBuffer(GL_BACK);
    glReadPixels(viewport[0] + static_cast<int>(208.0f / logicalSize.x * viewport[2]),
                 viewport[1] + static_cast<int>((logicalSize.y - 244.0f) / logicalSize.y * viewport[3]),
                 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    for (int channel = 0; channel < 3; ++channel)
    {
        const int expected = static_cast<int>(std::lround(warningColor[channel] * 255.0f * warningColor.a +
            std::lround(background[channel] * 255.0f) * (1.0f - warningColor.a)));
        if (std::abs(static_cast<int>(pixel[channel]) - expected) > 2) return false;
    }
    std::cout << "Aviso do ladino: quad colorido e alpha OK.\n";
    for (const Sample &sample : samples)
    {
        Texture texture;
        const std::string path = assets + sample.path;
        if (!texture.load(path)) return false;
        for (int frame = 0; frame < sample.frames; ++frame)
        {
            const glm::vec2 position(160.0f + frame * 8.0f, 220.0f);
            const glm::vec2 size(128.0f, 128.0f);
            glClearColor(background.r, background.g, background.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            renderer.draw(texture, position, size,
                {static_cast<float>(frame + (sample.flipX ? 1 : 0)) / sample.frames, 0.0f},
                {(sample.flipX ? -1.0f : 1.0f) / sample.frames, 1.0f}, sample.angle);
            std::cout << "Quadro " << frame << '/' << sample.frames << ": ";
            if (!checkSpritePixels(path, position, size, logicalSize, background,
                                   frame, sample.frames, sample.angle, sample.flipX))
                return false;
        }
    }
    return glGetError() == GL_NO_ERROR;
}

bool checkPlayerInput(Game &game)
{
    if (game.getState() != GameState::Menu) return false;
    game.update(2);
    game.handleKey(GLFW_KEY_W, GLFW_PRESS);
    game.handleKey(GLFW_KEY_SPACE, GLFW_PRESS);
    game.handleKey(GLFW_KEY_ENTER, GLFW_REPEAT);
    if (game.getCombat().distance != 0 || game.getPlayer().attacking ||
        game.getPlayer().lane != 1 || game.getState() != GameState::Menu) return false;
    game.handleKey(GLFW_KEY_ENTER, GLFW_PRESS);
    if (game.getState() != GameState::Playing) return false;
    game.handleKey(GLFW_KEY_H, GLFW_REPEAT);
    if (game.hitboxesVisible()) return false;
    game.handleKey(GLFW_KEY_H, GLFW_PRESS);
    if (!game.hitboxesVisible()) return false;
    game.handleKey(GLFW_KEY_W, GLFW_REPEAT);
    game.handleKey(GLFW_KEY_W, GLFW_RELEASE);
    if (game.getPlayer().lane != 1) return false;
    game.handleKey(GLFW_KEY_W, GLFW_PRESS);
    game.handleKey(GLFW_KEY_UP, GLFW_PRESS);
    if (game.getPlayer().lane != 0) return false;
    game.handleKey(GLFW_KEY_DOWN, GLFW_PRESS);
    game.handleKey(GLFW_KEY_DOWN, GLFW_REPEAT);
    if (game.getPlayer().lane != 1) return false;
    game.handleKey(GLFW_KEY_S, GLFW_PRESS);
    game.handleKey(GLFW_KEY_S, GLFW_PRESS);
    if (game.getPlayer().lane != 2) return false;
    game.handleKey(GLFW_KEY_UP, GLFW_PRESS);
    game.handleKey(GLFW_KEY_SPACE, GLFW_REPEAT);
    if (game.getPlayer().attacking) return false;
    game.handleKey(GLFW_KEY_SPACE, GLFW_PRESS);
    if (!game.getPlayer().attacking) return false;
    const double beforePause = game.getCombat().distance;
    const double cooldown = game.getPlayer().cooldownRemaining;
    game.handleKey(GLFW_KEY_P, GLFW_REPEAT);
    if (game.isPaused()) return false;
    game.handleKey(GLFW_KEY_P, GLFW_PRESS);
    game.update(5);
    game.handleKey(GLFW_KEY_W, GLFW_PRESS);
    game.handleKey(GLFW_KEY_SPACE, GLFW_PRESS);
    game.handleKey(GLFW_KEY_ENTER, GLFW_PRESS);
    if (!game.isPaused() || game.getCombat().distance != beforePause ||
        game.getPlayer().cooldownRemaining != cooldown || game.getPlayer().lane != 1 ||
        game.getPlayer().slash.currentFrame != 0 || !game.getEncounters().empty()) return false;
    game.handleKey(GLFW_KEY_M, GLFW_PRESS);
    if (!game.isMuted()) return false;
    game.handleKey(GLFW_KEY_M, GLFW_REPEAT);
    if (!game.isMuted()) return false;
    game.handleKey(GLFW_KEY_M, GLFW_PRESS);
    game.handleKey(GLFW_KEY_P, GLFW_PRESS);
    if (game.isPaused() || game.isMuted()) return false;
    game.handleKey(GLFW_KEY_9, GLFW_PRESS);
    if (game.getVolume() != 90) return false;
    game.handleKey(GLFW_KEY_0, GLFW_REPEAT);
    if (game.getVolume() != 90) return false;
    for (int i = 0; i < 15; ++i) game.handleKey(GLFW_KEY_9, GLFW_PRESS);
    if (game.getVolume() != 0) return false;
    for (int i = 0; i < 15; ++i) game.handleKey(GLFW_KEY_0, GLFW_PRESS);
    if (game.getVolume() != 100) return false;
    game.update(0.10);
    game.handleKey(GLFW_KEY_SPACE, GLFW_PRESS);
    if (!game.getPlayer().attackActive() || game.getPlayer().slash.currentFrame != 2) return false;
    game.update(0.50);
    game.handleKey(GLFW_KEY_SPACE, GLFW_REPEAT);
    if (game.getPlayer().attacking) return false;
    std::cout << "Input OK: linhas, ataque, pausa sem avancar timers, retomada e mute.\n";
    return true;
}

bool checkWorldScrolling(const Renderer2D &renderer, const std::string &assets,
                          glm::vec3 background)
{
    World world;
    if (!world.initialize(assets)) return false;
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] <= 0 || viewport[3] <= 0) return false;
    const auto capture = [&]()
    {
        glClearColor(background.r, background.g, background.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        world.draw(renderer);
        std::vector<unsigned char> pixels(static_cast<std::size_t>(viewport[2]) * viewport[3] * 4);
        glReadBuffer(GL_BACK);
        glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        return pixels;
    };
    const auto initial = capture();
    world.update(0.5);
    const auto moved = capture();
    int changedScenery = 0, changedGround = 0, groundGaps = 0;
    for (int y = 0; y < viewport[3]; ++y)
        for (int x = 0; x < viewport[2]; ++x)
        {
            const float worldY = (viewport[3] - y - 0.5f) * 600.0f / viewport[3];
            const std::size_t index = (static_cast<std::size_t>(y) * viewport[2] + x) * 4;
            bool changed = false, sky = true;
            for (int channel = 0; channel < 3; ++channel)
            {
                changed = changed || initial[index + channel] != moved[index + channel];
                sky = sky && std::abs(static_cast<int>(moved[index + channel]) -
                    static_cast<int>(std::lround(background[channel] * 255.0f))) <= 2;
            }
            if (worldY < 176.0f && changed) ++changedScenery;
            if (worldY >= 176.0f && changed) ++changedGround;
            if (worldY >= 176.0f && sky) ++groundGaps;
        }
    // 20480 pixels: 4 voltas da vila, 9 das arvores e 640 dos tiles do chao.
    // Tempo grande so no teste; a execucao normal limita deltaTime a 0.05 s.
    world.update(20480.0 / World::baseSpeed - 0.5);
    const auto repeated = capture();
    const bool valid = initial == repeated && changedScenery > 0 && changedGround > 0 &&
                       groundGaps == 0 && glGetError() == GL_NO_ERROR;
    std::cout << "Cenario GPU: " << (valid ? "OK" : "FALHOU")
              << "; ciclo identico: " << (initial == repeated)
              << "; pixels movidos (fundo/chao): " << changedScenery << '/' << changedGround
              << "; frestas no chao: " << groundGaps << '\n';
    return valid;
}

bool checkCombatIntegration(Game &game, const Renderer2D &renderer)
{
    // Segue um corpo da primeira onda ate o contato, usando apenas input real do jogo.
    for (int step = 0; step < 1200 && game.getCombat().alive(); ++step)
    {
        const auto &entities = game.getEncounters();
        int target = game.getPlayer().lane;
        if (!entities.enemies.empty()) target = entities.enemies.front().lane;
        else if (!entities.obstacles.empty()) target = entities.obstacles.front().lane;
        while (game.getPlayer().lane != target)
            game.handleKey(game.getPlayer().lane < target ? GLFW_KEY_S : GLFW_KEY_W, GLFW_PRESS);
        game.update(0.05);
    }
    if (game.getCombat().alive() || game.getCombat().lives != 0 || game.getCombat().distance <= 0) return false;
    const double distance = game.getCombat().distance;
    const int lane = game.getPlayer().lane;
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    const auto capture = [&]()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        game.draw(renderer);
        std::vector<unsigned char> pixels(static_cast<std::size_t>(viewport[2]) * viewport[3] * 4);
        glReadBuffer(GL_BACK);
        glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        return pixels;
    };
    const auto before = capture();
    // Vida perdida: a imagem do HUD nao pode conter o vermelho de meio coracao.
    int grayPixels = 0;
    for (int y = 0; y < viewport[3]; ++y)
        for (int x = 0; x < viewport[2]; ++x)
        {
            const float wx = (x + 0.5f) * 800 / viewport[2];
            const float wy = (viewport[3] - y - 0.5f) * 600 / viewport[3];
            if (wx < 24 || wx >= 56 || wy < 40 || wy >= 72) continue;
            const auto index = (static_cast<std::size_t>(y) * viewport[2] + x) * 4;
            const int r = before[index], g = before[index + 1], b = before[index + 2];
            if (r > g + 25 && r > b + 25) return false;
            if (r > 90 && std::abs(r - g) < 20 && std::abs(g - b) < 20) ++grayPixels;
        }
    if (grayPixels == 0) return false;
    unsigned char hudPixel[4] = {};
    glReadPixels(viewport[0] + static_cast<int>(237.0f / 800 * viewport[2]),
        viewport[1] + static_cast<int>(577.0f / 600 * viewport[3]), 1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE, hudPixel);
    if (hudPixel[0] < 253 || hudPixel[1] < 240 || hudPixel[2] < 202) return false;
    // O contorno ciano do jogador deve aparecer sobre o sprite.
    const Rect box = playerBox(game.getPlayer());
    unsigned char pixel[4] = {};
    glReadPixels(viewport[0] + static_cast<int>((box.x + 1) / 800 * viewport[2]),
        viewport[1] + static_cast<int>((600 - box.y - 1) / 600 * viewport[3]),
        1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    if (pixel[0] > 2 || pixel[1] < 253 || pixel[2] < 253) return false;
    game.handleKey(lane == 0 ? GLFW_KEY_S : GLFW_KEY_W, GLFW_PRESS);
    game.handleKey(GLFW_KEY_SPACE, GLFW_PRESS);
    game.update(1);
    if (game.getCombat().distance != distance || game.getPlayer().lane != lane || before != capture()) return false;
    game.handleKey(GLFW_KEY_H, GLFW_PRESS);
    if (game.hitboxesVisible() || before == capture()) return false;
    if (game.getState() != GameState::GameOver) return false;
    game.handleKey(GLFW_KEY_L, GLFW_PRESS);
    if (game.getState() != GameState::Shop) return false;
    game.handleKey(GLFW_KEY_1, GLFW_PRESS);
    if (game.windowTitle().find("Ouro insuficiente") == std::string::npos) return false;
    capture(); // Inclui os tres icones e quads da loja no teste OpenGL.
    game.update(1);
    if (game.getCombat().distance != distance) return false;
    game.handleKey(GLFW_KEY_ENTER, GLFW_PRESS);
    if (game.getState() != GameState::Playing || game.getCombat().lives != 1 ||
        game.getCombat().distance != 0 || game.getCombat().runCoins != 0 ||
        !game.getEncounters().empty() || !game.getCombat().coins.empty() ||
        game.getPlayer().attacking || game.getPlayer().cooldownRemaining != 0 ||
        game.getEncounters().speed() != Encounters::initialSpeed || game.getVolume() != 100) return false;
    game.update(0.1);
    if (game.getCombat().distance <= 0) return false;
    std::cout << "Integracao: menu, dano, game over, hitboxes, loja e reinicio OK.\n";
    return glGetError() == GL_NO_ERROR;
}
