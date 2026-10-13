#include "Player.h"
#include <algorithm>

void Player::changeLane(int direction)
{
    lane = std::clamp(lane + direction, 0, 2);
}

bool Player::tryAttack()
{
    if (attacking || cooldownRemaining > 1e-9)
        return false;
    attacking = true;
    cooldownRemaining = cooldownDuration;
    slash.reset();
    return true;
}

void Player::update(double deltaTime)
{
    if (deltaTime <= 0.0)
        return;
    run.update(deltaTime);
    cooldownRemaining = std::max(0.0, cooldownRemaining - deltaTime);
    if (attacking)
    {
        slash.update(deltaTime);
        if (slash.finished)
            attacking = false;
    }
}

bool Player::attackActive() const
{
    return attacking && slash.currentFrame >= 2 && slash.currentFrame <= 3;
}
