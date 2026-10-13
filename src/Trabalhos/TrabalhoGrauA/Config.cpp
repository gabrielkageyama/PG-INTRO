#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <locale>

GameConfig readConfig(std::istream &input, std::ostream &warnings)
{
    GameConfig result;
    double prices[] = {10, 15, 20};
    struct Field { const char *name; double *value; double minimum, maximum; bool integer = false; };
    const Field fields[] = {
        {"initial_speed", &result.initialSpeed, 100, 320},
        {"maximum_speed", &result.maximumSpeed, 100, 320},
        {"acceleration", &result.acceleration, 0, 10},
        {"spawn_initial", &result.initialInterval, 0.15, 3},
        {"spawn_minimum", &result.minimumInterval, 0.05, 3},
        {"spawn_decay", &result.intervalDecay, 0, 1},
        {"attack_cooldown", &result.attackCooldown, 0.3, 2},
        {"attack_frame", &result.attackFrame, 0.025, 0.08},
        {"price_range", &prices[0], 1, 1000, true},
        {"price_life", &prices[1], 1, 1000, true},
        {"price_gold", &prices[2], 1, 1000, true}
    };
    std::string line;
    int number = 0;
    while (std::getline(input, line))
    {
        ++number;
        line = line.substr(0, line.find('#'));
        if (line.find_first_not_of(" \t\r") == std::string::npos) continue;
        const auto equals = line.find('=');
        std::string key = line.substr(0, equals);
        const auto first = key.find_first_not_of(" \t\r");
        if (first != std::string::npos)
            key = key.substr(first, key.find_last_not_of(" \t\r") - first + 1);
        bool valid = false;
        if (equals != std::string::npos)
        {
            std::istringstream valueText(line.substr(equals + 1));
            valueText.imbue(std::locale::classic());
            double value = 0;
            if (valueText >> value)
            {
                valueText >> std::ws;
                if (valueText.eof() && std::isfinite(value))
                    for (const auto &field : fields)
                        if (key == field.name && value >= field.minimum && value <= field.maximum &&
                            (!field.integer || value == std::floor(value)))
                        { *field.value = value; valid = true; break; }
            }
        }
        if (!valid) warnings << "Config: linha " << number << " ignorada (chave ou valor invalido).\n";
    }
    const GameConfig defaults;
    if (result.initialSpeed > result.maximumSpeed)
    {
        warnings << "Config: velocidades incoerentes; usando padroes.\n";
        result.initialSpeed = defaults.initialSpeed; result.maximumSpeed = defaults.maximumSpeed;
    }
    if (result.minimumInterval > result.initialInterval)
    {
        warnings << "Config: intervalos incoerentes; usando padroes.\n";
        result.initialInterval = defaults.initialInterval; result.minimumInterval = defaults.minimumInterval;
    }
    if (6 * result.attackFrame > result.attackCooldown)
    {
        warnings << "Config: ataque maior que cooldown; usando padroes.\n";
        result.attackFrame = defaults.attackFrame; result.attackCooldown = defaults.attackCooldown;
    }
    for (int i = 0; i < 3; ++i) result.prices[i] = static_cast<int>(prices[i]);
    return result;
}
GameConfig loadConfig(const std::string &path)
{
    std::ifstream input(path);
    if (!input) { std::cerr << "Config ausente; usando valores padrao.\n"; return {}; }
    return readConfig(input, std::cerr);
}
