#include "Combat.h"
#include <algorithm>

bool Combat::takeDamage()
{
    if (!alive() || invulnerability > 1e-9) return false;
    --lives;
    playerHurt = true;
    invulnerability = invulnerabilityDuration;
    return true;
}

void Combat::update(double deltaTime, const Player &player, Encounters &encounters)
{
    enemyDefeated = playerHurt = obstacleHurt = collected = false;
    if (!alive() || deltaTime <= 0.0) return;
    invulnerability = std::max(0.0, invulnerability - deltaTime);
    const float movement = static_cast<float>(encounters.speed() * deltaTime);
    distance += encounters.speed() * deltaTime;
    for (Coin &coin : coins)
    {
        coin.x -= movement;
        coin.animation.update(deltaTime);
    }
    resolve(player, encounters);
}

void Combat::resolve(const Player &player, Encounters &encounters)
{
    if (!alive()) return;
    const bool hadEntities = !encounters.empty();
    // O slash vence o contato no mesmo passo. Cada inimigo gera uma moeda.
    if (player.attackActive())
    {
        const Rect attack = attackBox(player);
        encounters.enemies.erase(std::remove_if(encounters.enemies.begin(), encounters.enemies.end(),
            [&](const Enemy &enemy)
            {
                if (!overlaps(attack, enemyBox(enemy))) return false;
                coins.push_back({enemy.lane, enemy.x});
                enemyDefeated = true;
                return true;
            }), encounters.enemies.end());
    }
    const Rect body = playerBox(player);
    for (const Enemy &enemy : encounters.enemies)
    {
        if (overlaps(body, enemyBox(enemy))) takeDamage();
        if (enemy.type == EnemyType::Rogue && enemy.phase == RoguePhase::Active &&
            enemy.targetLane >= 0 && overlaps(body, rogueBox(enemy))) takeDamage();
    }
    for (const Obstacle &obstacle : encounters.obstacles)
        if (overlaps(body, obstacleBox(obstacle)) && takeDamage()) obstacleHurt = true;

    // Mesmo durante invulnerabilidade, um projetil que toca o jogador e consumido.
    encounters.projectiles.erase(std::remove_if(encounters.projectiles.begin(), encounters.projectiles.end(),
        [&](const Projectile &projectile)
        {
            if (!overlaps(body, projectileBox(projectile))) return false;
            takeDamage();
            return true;
        }), encounters.projectiles.end());
    if (hadEntities && encounters.empty()) encounters.onWaveCleared();

    coins.erase(std::remove_if(coins.begin(), coins.end(), [&](const Coin &coin)
        {
            if (coin.x + 16 < 0) return true;
            if (!alive() || !overlaps(body, coinBox(coin))) return false;
            ++runCoins;
            collected = true;
            gold += coinValue;
            runGold += coinValue;
            return true;
        }), coins.end());
}
