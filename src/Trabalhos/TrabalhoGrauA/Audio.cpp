#include "Audio.h"
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

#ifdef GRAU_A_AUDIO
#include <miniaudio.h>
struct Audio::Impl
{
    ma_engine engine{};
    std::array<ma_sound, static_cast<int>(Sound::Count)> sounds{};
    int loaded = 0;
    bool initialized = false;
    bool offline = false;
    ~Impl()
    {
        for (int i = 0; i < loaded; ++i) ma_sound_uninit(&sounds[i]);
        if (initialized) ma_engine_uninit(&engine);
    }
};
#else
struct Audio::Impl {};
#endif

Audio::Audio() : impl(std::make_unique<Impl>()) {}
Audio::~Audio() = default;
bool Audio::ready() const
{
#ifdef GRAU_A_AUDIO
    return impl->initialized && impl->loaded == static_cast<int>(Sound::Count);
#else
    return false;
#endif
}
bool Audio::initialize(const std::string &assets, bool offline)
{
#ifdef GRAU_A_AUDIO
    impl = std::make_unique<Impl>();
    auto config = ma_engine_config_init();
    config.noDevice = offline;
    config.channels = 2;
    config.sampleRate = 48000;
    if (ma_engine_init(&config, &impl->engine) != MA_SUCCESS)
    {
        std::cerr << "Audio indisponivel; jogo continua silencioso.\n";
        return false;
    }
    impl->initialized = true;
    impl->offline = offline;
    const char *files[] = {"music/medieval-run-loop.ogg", "sfx/sword-slash.wav", "sfx/fireball-cast.wav",
        "sfx/enemy-hit.ogg", "sfx/player-hit.ogg", "sfx/coin-pickup.ogg", "sfx/shop-purchase.ogg",
        "sfx/obstacle-hit.ogg", "sfx/game-over.ogg"};
    for (const char *file : files)
    {
        const std::string path = assets + "/audio/" + file;
        if (ma_sound_init_from_file(&impl->engine, path.c_str(), MA_SOUND_FLAG_DECODE |
            MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, nullptr, &impl->sounds[impl->loaded]) != MA_SUCCESS)
        {
            std::cerr << "Falha no audio " << path << "; jogo continua silencioso.\n";
            impl = std::make_unique<Impl>();
            return false;
        }
        ++impl->loaded;
    }
    ma_sound_set_looping(&impl->sounds[0], MA_TRUE);
    ma_sound_set_volume(&impl->sounds[0], 0.18f);
    for (int i = 1; i < impl->loaded; ++i) ma_sound_set_volume(&impl->sounds[i], 0.45f);
    std::cout << "Audio: 9 arquivos carregados (" << (offline ? "teste sem dispositivo" : "dispositivo ativo") << ").\n";
    return true;
#else
    (void)assets; (void)offline;
    std::cout << "Audio desabilitado no build.\n";
    return false;
#endif
}
void Audio::play(Sound sound)
{
#ifdef GRAU_A_AUDIO
    if (!ready() || sound == Sound::Count) return;
    auto *clip = &impl->sounds[static_cast<int>(sound)];
    ma_sound_stop(clip);
    ma_sound_seek_to_pcm_frame(clip, 0);
    ma_sound_start(clip);
#else
    (void)sound;
#endif
}
void Audio::stop(Sound sound)
{
#ifdef GRAU_A_AUDIO
    if (ready() && sound != Sound::Count) ma_sound_stop(&impl->sounds[static_cast<int>(sound)]);
#else
    (void)sound;
#endif
}
void Audio::beginRun()
{
    for (int i = 0; i < static_cast<int>(Sound::Count); ++i) stop(static_cast<Sound>(i));
    play(Sound::Music);
}
void Audio::pause(bool paused)
{
#ifdef GRAU_A_AUDIO
    if (!ready()) return;
    if (paused) ma_engine_stop(&impl->engine);
    else ma_engine_start(&impl->engine);
#else
    (void)paused;
#endif
}
void Audio::mute(bool muted)
{
    setVolume(muted ? 0 : 100);
}
void Audio::setVolume(int percent)
{
#ifdef GRAU_A_AUDIO
    if (ready()) ma_engine_set_volume(&impl->engine, std::clamp(percent, 0, 100) / 100.0f);
#else
    (void)percent;
#endif
}
bool Audio::checkOffline()
{
#ifdef GRAU_A_AUDIO
    if (!ready() || !impl->offline) return false;
    std::array<float, 1024 * 2> pcm{};
    for (int i = 0; i < impl->loaded; ++i)
    {
        for (int j = 0; j < impl->loaded; ++j) stop(static_cast<Sound>(j));
        play(static_cast<Sound>(i));
        double energy = 0;
        for (int block = 0; block < 100; ++block)
        {
            if (ma_engine_read_pcm_frames(&impl->engine, pcm.data(), 1024, nullptr) != MA_SUCCESS) return false;
            for (float sample : pcm) { if (!std::isfinite(sample)) return false; energy += std::abs(sample); }
        }
        if (energy < 0.01) return false;
    }
    setVolume(50);
    if (std::abs(ma_engine_get_volume(&impl->engine) - 0.5f) > 0.001f) return false;
    setVolume(200);
    if (ma_engine_get_volume(&impl->engine) != 1.0f) return false;
    setVolume(-10);
    if (ma_engine_get_volume(&impl->engine) != 0.0f) return false;
    play(Sound::Music);
    if (ma_engine_read_pcm_frames(&impl->engine, pcm.data(), 1024, nullptr) != MA_SUCCESS) return false;
    for (float sample : pcm) if (sample != 0) return false;
    std::cout << "Audio: WAV/OGG, nove sons, mixagem e mute OK.\n";
    return true;
#else
    return false;
#endif
}
