#ifndef GRAU_A_HUD_H
#define GRAU_A_HUD_H
#include "Renderer2D.h"
#include "Texture.h"
#include "Combat.h"
#include "Progression.h"

class Hud
{
public:
    bool initialize(const std::string &assets);
    void draw(const Renderer2D &renderer, const Combat &combat, int maxLives,
              const Player &player, bool paused, int volume, bool audioAvailable) const;
    void drawControls(const Renderer2D &renderer, GameState state, bool paused) const;
    void drawShop(const Renderer2D &renderer, const Progression &progression, const Combat &combat,
                  const std::array<Texture, 3> &icons, const std::string &message) const;
    static void text(const Renderer2D &renderer, const std::string &value,
                     float x, float y, float scale = 2.0f);
private:
    Texture fullHeart;
    Texture emptyHeart;
};
#endif
