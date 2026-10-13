#include "World.h"
#include "Player.h"
#include <iostream>

bool World::initialize(const std::string &assets)
{
    if (!atlas.load(assets + "/environment/tiny-town-tileset-16x16.png") ||
        !grass.load(assets + "/environment/grass-16x16.png") ||
        !dirt.load(assets + "/environment/dirt-path-16x16.png"))
        return false;
    if (atlas.width() != 203 || atlas.height() != 186 ||
        grass.width() != 16 || grass.height() != 16 || dirt.width() != 16 || dirt.height() != 16)
    {
        std::cerr << "Dimensoes inesperadas nos assets do cenario.\n";
        return false;
    }
    return true;
}

void World::update(double deltaTime, double speed)
{
    village.update(deltaTime, speed);
    trees.update(deltaTime, speed);
    ground.update(deltaTime, speed);
}

void World::drawTile(const Renderer2D &renderer, int column, int row,
                      float x, float y, float size) const
{
    // Atlas: tiles 16x16, separados por 1 pixel
    renderer.draw(atlas, {x, y}, {size, size},
        {column * 17.0f / atlas.width(), row * 17.0f / atlas.height()},
        {16.0f / atlas.width(), 16.0f / atlas.height()});
}

void World::drawHouse(const Renderer2D &renderer, float x) const
{
    // Casa de 3x3 tiles: telhado, beiral e parede com porta
    const int tiles[3][3] = {{48, 49, 50}, {60, 63, 62}, {72, 74, 75}};
    for (int row = 0; row < 3; ++row)
        for (int column = 0; column < 3; ++column)
        {
            const int tile = tiles[row][column];
            drawTile(renderer, tile % 12, tile / 12, x + column * 32.0f,
                      72.0f + row * 32.0f, 32.0f);
        }
}

void World::drawTree(const Renderer2D &renderer, float x, float baseline, float tileSize) const
{
    drawTile(renderer, 4, 0, x, baseline - 2.0f * tileSize, tileSize);
    drawTile(renderer, 4, 1, x, baseline - tileSize, tileSize);
}

void World::draw(const Renderer2D &renderer) const
{
    // Vila distante: duas copias de um padrao de 1024 px, maior que a tela
    for (int copy = 0; copy < 2; ++copy)
    {
        const float x = village.copyX(copy);
        for (float houseX : {80.0f, 416.0f, 784.0f}) drawHouse(renderer, x + houseX);
        for (float treeX : {16.0f, 288.0f, 656.0f, 960.0f})
            drawTree(renderer, x + treeX, 168.0f, 32.0f);
    }

    // Arvores proximas: maiores, sobre a vila, e mais rapidas
    for (int copy = 0; copy < 2; ++copy)
    {
        const float x = trees.copyX(copy);
        for (float treeX : {0.0f, 216.0f, 352.0f, 576.0f, 736.0f, 944.0f})
            drawTree(renderer, x + treeX, 176.0f, 48.0f);
    }

    // Chao: tiles repetidos, incluindo um extra na borda direita
    const float startX = ground.copyX(0);
    for (float y = 176.0f; y < 600.0f; y += 32.0f)
        for (float x = startX; x < 800.0f; x += 32.0f)
            renderer.draw(grass, {x, y}, {32.0f, 32.0f});

    // O centro 8x8 forma o piso, sem repetir cantos verdes no meio da estrada
    // Mantem o centro visual anterior (laneY + 16), com metade da altura
    for (float laneY : Player::laneCenters)
        for (int row = 0; row < 2; ++row)
        {
            const float height = row == 0 ? 32.0f : 16.0f;
            for (float x = startX; x < 800.0f; x += 32.0f)
                // Recorta a ultima metade do tile em vez de comprimir sua textura.
                renderer.draw(dirt, {x, laneY - 8.0f + row * 32.0f}, {32.0f, height},
                    {0.25f, 0.25f}, {0.5f, height / 64.0f});
        }
}
