#ifndef GRAU_A_ENCOUNTERS_H
#define GRAU_A_ENCOUNTERS_H

#include "Sprite.h"
#include "Config.h"
#include <random>
#include <vector>

enum class EnemyType { Orc, Mage, Rogue };
enum class RoguePhase { Ready, Warning, Active, Spent };

struct Enemy
{
    EnemyType type;
    int lane;
    float x;
    SpriteAnimation animation{7, 0.10, true};
    bool hasFired = false;
    int targetLane = -1;
    RoguePhase phase = RoguePhase::Ready;
    double phaseTimer = 0.0;
};

struct Obstacle
{
    int lane;
    int variant; // caixa, pedra, barril, espinhos, carroca, feno
    float x;
};

struct Projectile
{
    int lane;
    float x;
    SpriteAnimation animation{4, 0.08, true};
};

// Logica de entidades e dificuldade, sem GLFW/OpenGL.
class Encounters
{
public:
    explicit Encounters(unsigned int seed = std::random_device{}(), GameConfig config = {})
        : random(seed), settings(config) {}
    void update(double deltaTime);
    double speed() const;
    double spawnInterval() const;
    void onWaveCleared() { nextWave = spawnInterval(); }
    int reservedLane() const { return safeLane; }
    int waveCount() const { return waves; }
    bool empty() const { return enemies.empty() && obstacles.empty() && projectiles.empty(); }

    static constexpr double initialSpeed = 180.0;
    static constexpr double maximumSpeed = 320.0;
    static constexpr double acceleration = 2.0;
    static constexpr double warningDuration = 0.65;
    static constexpr double activeDuration = 0.35;
    static constexpr double projectileExtraSpeed = 140.0;
    std::vector<Enemy> enemies;
    std::vector<Obstacle> obstacles;
    std::vector<Projectile> projectiles;
    bool firedThisStep = false;

private:
    std::mt19937 random;
    GameConfig settings;
    double elapsed = 0.0;
    double nextWave = 1.0;
    int safeLane = 1;
    int waves = 0;
    void spawnWave();
    int choose(int maximum);
};

#endif
