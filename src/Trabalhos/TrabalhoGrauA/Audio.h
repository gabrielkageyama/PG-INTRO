#ifndef GRAU_A_AUDIO_H
#define GRAU_A_AUDIO_H
#include <memory>
#include <string>

enum class Sound { Music, Slash, Fireball, EnemyHit, PlayerHit, Coin, Purchase, Obstacle, GameOver, Count };

class Audio
{
public:
    Audio();
    ~Audio();
    Audio(const Audio &) = delete;
    Audio &operator=(const Audio &) = delete;
    bool initialize(const std::string &assets, bool offline = false);
    void play(Sound sound);
    void stop(Sound sound);
    void beginRun();
    void pause(bool paused);
    void mute(bool muted);
    void setVolume(int percent);
    bool ready() const;
    bool checkOffline();
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
#endif
