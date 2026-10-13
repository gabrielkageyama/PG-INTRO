#include "Encounters.h"
#include "Player.h"
#include <algorithm>

int Encounters::choose(int maximum)
{
    return std::uniform_int_distribution<int>(0, maximum)(random);
}

double Encounters::speed() const
{
    return std::min(settings.maximumSpeed, settings.initialSpeed + settings.acceleration * elapsed);
}

double Encounters::spawnInterval() const
{
    return std::max(settings.minimumInterval, settings.initialInterval - settings.intervalDecay * elapsed);
}

void Encounters::spawnWave()
{
    safeLane = choose(1) * 2;
    int lanes[2] = {};
    int count = 0;
    for (int lane = 0; lane < 3; ++lane)
        if (lane != safeLane) lanes[count++] = lane;
    float nextX = 848.0f;
    float lastX[3] = {};
    const auto place = [&](int lane)
    {
        const float gap = static_cast<float>(std::max(240.0,
            settings.maximumSpeed * (settings.attackCooldown + 0.21)));
        const float x = std::max(nextX, lastX[lane] + gap);
        lastX[lane] = x;
        nextX = x + 120.0f;
        return x;
    };
    for (int group = 0; group < 3; ++group)
    {
        if (choose(1) == 1) std::swap(lanes[0], lanes[1]);
        const int pattern = choose(3);
        const float entryX = place(lanes[0]);
        if (pattern == 0)
            obstacles.push_back({lanes[0], choose(5), entryX});
        else
        {
            Enemy enemy{static_cast<EnemyType>(pattern - 1), lanes[0], entryX};
            if (enemy.type == EnemyType::Rogue) enemy.targetLane = lanes[1];
            enemies.push_back(enemy);
        }
        obstacles.push_back({lanes[1], choose(5), place(lanes[1])});
    }
    ++waves;
}

void Encounters::update(double deltaTime)
{
    firedThisStep = false;
    if (deltaTime <= 0.0) return;
    elapsed += deltaTime;
    if (empty())
    {
        nextWave -= deltaTime;
        if (nextWave <= 0.0) spawnWave();
    }
    const bool hadEntities = !empty();
    const float movement = static_cast<float>(speed() * deltaTime);
    for (Obstacle &obstacle : obstacles) obstacle.x -= movement;
    for (Enemy &enemy : enemies)
    {
        enemy.x -= movement;
        enemy.animation.update(deltaTime);
        if (enemy.type == EnemyType::Mage && !enemy.hasFired && enemy.x <= 640.0f)
        {
            projectiles.push_back({enemy.lane, enemy.x - 32.0f});
            enemy.hasFired = true;
            firedThisStep = true;
        }
        if (enemy.type != EnemyType::Rogue) continue;
        if (enemy.phase == RoguePhase::Ready)
        {
            if (enemy.x <= Player::centerX + speed() * warningDuration + 80.0)
            {
                enemy.phase = RoguePhase::Warning;
                enemy.phaseTimer = warningDuration;
            }
        }
        else if (enemy.phase == RoguePhase::Warning || enemy.phase == RoguePhase::Active)
        {
            enemy.phaseTimer -= deltaTime;
            if (enemy.phaseTimer <= 1e-9)
            {
                if (enemy.phase == RoguePhase::Warning)
                {
                    enemy.phase = RoguePhase::Active;
                    enemy.phaseTimer = activeDuration;
                }
                else enemy.phase = RoguePhase::Spent;
            }
        }
    }
    for (Projectile &projectile : projectiles)
    {
        projectile.x -= static_cast<float>((speed() + projectileExtraSpeed) * deltaTime);
        projectile.animation.update(deltaTime);
    }
    // Remove somente depois das iteracoes; nenhuma referencia fica invalidada nelas.
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy &enemy) { return enemy.x + 48.0f < 0.0f; }), enemies.end());
    obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(),
        [](const Obstacle &obstacle) { return obstacle.x + 32.0f < 0.0f; }), obstacles.end());
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const Projectile &projectile) { return projectile.x + 32.0f < 0.0f; }), projectiles.end());
    if (hadEntities && empty()) nextWave = spawnInterval();
}
