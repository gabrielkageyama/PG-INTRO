#ifndef GRAU_A_WORLD_H
#define GRAU_A_WORLD_H

#include "Renderer2D.h"
#include "ScrollLayer.h"
#include "Texture.h"
#include <string>

// Cenario decorativo: nenhum elemento daqui e obstaculo ou causa dano.
class World
{
public:
    static constexpr double baseSpeed = 180.0;
    bool initialize(const std::string &assets);
    void update(double deltaTime, double speed = baseSpeed);
    void draw(const Renderer2D &renderer) const;
    void reset() { village.offset = trees.offset = ground.offset = 0.0; }

private:
    Texture atlas;
    Texture grass;
    Texture dirt;
    ScrollLayer village{1024.0, 0.20};
    ScrollLayer trees{1024.0, 0.45};
    ScrollLayer ground{32.0, 1.0};

    void drawTile(const Renderer2D &renderer, int column, int row,
                  float x, float y, float size) const;
    void drawHouse(const Renderer2D &renderer, float x) const;
    void drawTree(const Renderer2D &renderer, float x, float baseline, float tileSize) const;
};

#endif
