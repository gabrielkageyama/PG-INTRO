#ifndef GRAU_A_RENDER_CHECK_H
#define GRAU_A_RENDER_CHECK_H

#include <glm/glm.hpp>
#include <string>

class Renderer2D;
class Game;

// Usado apenas por --smoke-test; compara a imagem real da GPU com o PNG.
bool checkSpritePixels(const std::string &path, glm::vec2 position, glm::vec2 size,
                       glm::vec2 logicalSize, glm::vec3 background,
                       int frame = 0, int frameCount = 1, float angle = 0.0f, bool flipX = false);
bool checkSpriteSheets(const Renderer2D &renderer, const std::string &assets,
                       glm::vec2 logicalSize, glm::vec3 background);
bool checkPlayerInput(Game &game);
bool checkCombatIntegration(Game &game, const Renderer2D &renderer);
bool checkWorldScrolling(const Renderer2D &renderer, const std::string &assets,
                          glm::vec3 background);

#endif
