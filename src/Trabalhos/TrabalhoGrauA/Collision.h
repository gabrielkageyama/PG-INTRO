#ifndef GRAU_A_COLLISION_H
#define GRAU_A_COLLISION_H

#include "Player.h"
#include "Encounters.h"

struct Rect { float x, y, width, height; };

// Encostar somente nas bordas
inline bool overlaps(const Rect &a, const Rect &b)
{
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

inline Rect playerBox(const Player &p)
{ return {Player::centerX - 12, p.centerY() - 19, 24, 44}; }
inline Rect attackBox(const Player &p)
{ return {Player::centerX + 16, p.centerY() - 28, 88 + p.attackBonus, 64}; }
inline Rect enemyBox(const Enemy &e)
{ return {e.x - 16, Player::laneCenters[e.lane] - 24, 32, 54}; }
inline Rect obstacleBox(const Obstacle &o)
{ return {o.x - 22, Player::laneCenters[o.lane], 44, 36}; }
inline Rect projectileBox(const Projectile &p)
{ return {p.x - 12, Player::laneCenters[p.lane], 24, 20}; }
inline Rect rogueBox(const Enemy &e)
{ return {e.x - 48, Player::laneCenters[e.targetLane] - 8, 96, 48}; }

#endif
