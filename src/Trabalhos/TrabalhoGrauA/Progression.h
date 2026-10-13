#ifndef GRAU_A_PROGRESSION_H
#define GRAU_A_PROGRESSION_H

#include "Combat.h"
#include <array>

enum class GameState { Menu, Playing, GameOver, Shop };

struct Progression
{
    // Cinco compras por categoria mantem alcance e valores dentro de limites simples.
    static constexpr int maxLevel = 5;
    std::array<int, 3> levels{}; // alcance, vidas, ouro
    std::array<int, 3> basePrices{10, 15, 20};
    int price(int item) const
    {
        if (item < 0 || item >= 3 || levels[item] >= maxLevel) return 0;
        return basePrices[item] * (1 << levels[item]);
    }
    bool buy(int item, int &gold)
    {
        const int cost = price(item);
        if (cost == 0 || gold < cost) return false;
        gold -= cost;
        ++levels[item];
        return true;
    }
    void startRun(Player &player, Combat &combat) const
    {
        const int savings = combat.gold;
        player = Player{};
        player.attackBonus = 12.0f * levels[0];
        combat = Combat{};
        combat.gold = savings;
        combat.lives = 1 + levels[1];
        combat.coinValue = 1 + levels[2];
    }
};

#endif
