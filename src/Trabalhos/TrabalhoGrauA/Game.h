#ifndef GRAU_A_GAME_H
#define GRAU_A_GAME_H

#include "Player.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "World.h"
#include "Encounters.h"
#include "Combat.h"
#include "Progression.h"
#include "Hud.h"
#include "Audio.h"
#include <array>
#include <string>

// Coordena jogador, input, cenario, entidades e colisoes.
class Game
{
public:
    bool initialize(const std::string &assets);
    void handleKey(int key, int action);
    void update(double deltaTime);
    void draw(const Renderer2D &renderer) const;
    std::string windowTitle() const;
    const Player &getPlayer() const { return player; }
    const Encounters &getEncounters() const { return encounters; }
    const Combat &getCombat() const { return combat; }
    bool hitboxesVisible() const { return showHitboxes; }
    GameState getState() const { return state; }
    bool isPaused() const { return paused; }
    bool isMuted() const { return muted; }
    int getVolume() const { return muted ? 0 : volume; }

private:
    Player player;
    GameConfig config;
    Texture knight;
    Texture slash;
    World world;
    Encounters encounters;
    std::array<Texture, 3> enemyTextures;
    Texture obstacleTexture;
    Texture fireballTexture;
    Texture coinTexture;
    Combat combat;
    Hud hud;
    Audio audio;
    bool paused = false;
    bool muted = false;
    int volume = 100;
    Progression progression;
    GameState state = GameState::Menu;
    std::array<Texture, 3> shopTextures;
    std::string shopMessage;
    void startRun();
    bool showHitboxes = false;
    void drawEntities(const Renderer2D &renderer) const;
    void drawHitboxes(const Renderer2D &renderer) const;
};

#endif
