#ifndef GRAU_A_CONFIG_H
#define GRAU_A_CONFIG_H
#include <array>
#include <iosfwd>
#include <string>

struct GameConfig
{
    double initialSpeed = 180, maximumSpeed = 320, acceleration = 2;
    double initialInterval = 0.70, minimumInterval = 0.15, intervalDecay = 0.01;
    double attackCooldown = 0.54, attackFrame = 0.045;
    std::array<int, 3> prices{10, 15, 20};
};
GameConfig readConfig(std::istream &input, std::ostream &warnings);
GameConfig loadConfig(const std::string &path);
#endif
