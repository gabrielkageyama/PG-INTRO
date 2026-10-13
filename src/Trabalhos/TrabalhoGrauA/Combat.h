#ifndef GRAU_A_COMBAT_H
#define GRAU_A_COMBAT_H

#include "Collision.h"
#include <vector>

struct Coin
{
    int lane;
    float x;
    SpriteAnimation animation{4, 0.10, true};
};

inline Rect coinBox(const Coin &c)
{ return {c.x - 12, Player::laneCenters[c.lane] - 4, 24, 24}; }

struct Combat
{
    static constexpr double invulnerabilityDuration = 1.0;
    int lives = 1;
    int runCoins = 0;
    int runGold = 0;
    int gold = 0;
    int coinValue = 1;
    double distance = 0.0; // pixels percorridos pelo mundo
    double invulnerability = 0.0;
    std::vector<Coin> coins;
    // Escala de apresentacao: 100 pixels do mundo equivalem a 1 metro.
    int distanceMeters() const { return static_cast<int>(distance / 100.0); }
    // Eventos do passo atual, consumidos pelo audio
    bool enemyDefeated = false;
    bool playerHurt = false;
    bool obstacleHurt = false;
    bool collected = false;

    bool alive() const { return lives > 0; }
    bool takeDamage();
    void update(double deltaTime, const Player &player, Encounters &encounters);
    void resolve(const Player &player, Encounters &encounters);
};

#endif
