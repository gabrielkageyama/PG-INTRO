#ifndef GRAU_A_PLAYER_H
#define GRAU_A_PLAYER_H

#include "Sprite.h"
#include <array>

// Logica pura: nao depende de janela, OpenGL ou arquivos dos exercicios.
struct Player
{
    static constexpr std::array<float, 3> laneCenters = {260.0f, 340.0f, 420.0f};
    static constexpr float centerX = 160.0f;
    static constexpr float spriteSize = 96.0f;
    static constexpr double attackCooldown = 0.54;

    int lane = 1;
    float attackBonus = 0.0f;
    double cooldownDuration = attackCooldown;
    SpriteAnimation run{7, 0.10, true};
    SpriteAnimation slash{6, 0.045, false};
    bool attacking = false;
    double cooldownRemaining = 0.0;

    void changeLane(int direction);
    bool tryAttack();
    void update(double deltaTime);
    float centerY() const { return laneCenters[lane]; }
    // Janela efetiva: quadros 2 e 3 (0.09 <= tempo < 0.18 s).
    // Collision.h define a hitbox; Combat resolve os acertos.
    bool attackActive() const;
};

#endif
