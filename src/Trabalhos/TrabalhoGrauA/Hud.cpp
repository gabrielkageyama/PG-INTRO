#include "Hud.h"
#include <algorithm>
#include <cstdint>

bool Hud::initialize(const std::string &assets)
{
    return fullHeart.load(assets + "/ui/heart-full-32x32.png") &&
        emptyHeart.load(assets + "/ui/heart-empty-32x32.png") &&
        fullHeart.width() == 32 && fullHeart.height() == 32 &&
        emptyHeart.width() == 32 && emptyHeart.height() == 32;
}

void Hud::text(const Renderer2D &renderer, const std::string &value, float x, float y, float scale)
{
    // Fonte original 3x5: cada bit ligado desenha um pequeno quad, sem biblioteca ou fonte externa.
    static constexpr const char *alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ:-/";
    static constexpr std::uint16_t glyphs[] = {
        0b111101101101111, 0b010110010010111, 0b111001111100111, 0b111001111001111,
        0b101101111001001, 0b111100111001111, 0b111100111101111, 0b111001001001001,
        0b111101111101111, 0b111101111001111,
        0b010101111101101, 0b110101110101110, 0b111100100100111, 0b110101101101110,
        0b111100110100111, 0b111100110100100, 0b111100101101111, 0b101101111101101,
        0b111010010010111, 0b001001001101111, 0b101101110101101, 0b100100100100111,
        0b101111111101101, 0b101111111111101, 0b111101101101111, 0b111101111100100,
        0b111101101111001, 0b110101110101101, 0b111100111001111, 0b111010010010010,
        0b101101101101111, 0b101101101101010, 0b101101111111101, 0b101101010101101,
        0b101101010010010, 0b111001010100111,
        0b000010000010000, 0b000000111000000, 0b001001010100100
    };
    for (char c : value)
    {
        const char *found = std::find(alphabet, alphabet + 39, c);
        if (found != alphabet + 39)
        {
            const auto bits = glyphs[found - alphabet];
            for (int row = 0; row < 5; ++row)
                for (int column = 0; column < 3; ++column)
                    if (bits & (1 << (14 - row * 3 - column)))
                        renderer.drawSolid({x + column * scale, y + row * scale},
                            {scale, scale}, {1, 0.95f, 0.8f, 1});
        }
        x += 4 * scale;
    }
}

void Hud::draw(const Renderer2D &renderer, const Combat &combat, int maxLives,
               const Player &player, bool paused, int volume, bool audioAvailable) const
{
    renderer.drawSolid({16, 12}, {768, 88}, {0.06f, 0.08f, 0.12f, 0.96f});
    text(renderer, "VIDAS " + std::to_string(combat.lives) + "/" + std::to_string(maxLives), 24, 22);
    for (int i = 0; i < maxLives; ++i)
    {
        const float x = 24.0f + i * 32;
        if (i < combat.lives) renderer.draw(fullHeart, {x, 40}, {32, 32});
        else
        {
            // O asset "empty" tem vermelho na metade esquerda. Usa apenas a metade
            // direita cinza, refletida para formar um coracao inteiramente vazio.
            renderer.draw(emptyHeart, {x, 40}, {16, 32}, {1, 0}, {-0.5f, 1});
            renderer.draw(emptyHeart, {x + 16, 40}, {16, 32}, {0.5f, 0}, {0.5f, 1});
        }
    }
    text(renderer, "OURO COLETADO", 236, 22);
    text(renderer, std::to_string(combat.runGold), 236, 40);
    text(renderer, "OURO TOTAL " + std::to_string(combat.gold), 236, 64);
    text(renderer, "DIST " + std::to_string(combat.distanceMeters()) + " M", 432, 22);
    text(renderer, "GOLPE", 432, 48);
    renderer.drawSolid({484, 46}, {100, 12}, {0.25f, 0.25f, 0.25f, 1});
    const float ready = static_cast<float>(1 - std::clamp(player.cooldownRemaining / player.cooldownDuration, 0.0, 1.0));
    if (ready > 0) renderer.drawSolid({484, 46}, {100 * ready, 12}, {0.3f, 0.8f, 0.4f, 1});
    text(renderer, audioAvailable ? "VOL " + std::to_string(volume) + "/100" : "SEM AUDIO", 624, 22);
    text(renderer, "9 BAIXAR", 624, 46);
    text(renderer, "0 AUMENTAR", 624, 68);
    if (paused)
    {
        renderer.drawSolid({208, 182}, {384, 66}, {0.06f, 0.08f, 0.12f, 0.96f});
        text(renderer, "PAUSADO", 358, 194, 3);
        text(renderer, "P PARA CONTINUAR", 340, 222, 2);
    }
}

void Hud::drawControls(const Renderer2D &renderer, GameState state, bool paused) const
{
    renderer.drawSolid({16, 518}, {768, 68}, {0.06f, 0.08f, 0.12f, 0.96f});
    std::string main;
    switch (state)
    {
    case GameState::Menu: main = "ENTER INICIAR CORRIDA"; break;
    case GameState::Playing: main = paused ? "P CONTINUAR CORRIDA" : "W/S MOVER   ESPACO ATACAR   P PAUSAR"; break;
    case GameState::GameOver: main = "FIM DA CORRIDA   L LOJA   ENTER REINICIAR"; break;
    case GameState::Shop: main = "1/2/3 COMPRAR   ENTER INICIAR CORRIDA"; break;
    }
    text(renderer, main, 32, 532);
    text(renderer, "ESC SAIR   H HITBOXES   9/0 VOLUME", 32, 558);
    if (state == GameState::Menu || state == GameState::Playing)
        text(renderer, "LOJA APOS A CORRIDA", 592, 558);
}

void Hud::drawShop(const Renderer2D &renderer, const Progression &progression, const Combat &combat,
                   const std::array<Texture, 3> &icons, const std::string &message) const
{
    renderer.drawSolid({96, 120}, {608, 378}, {0.06f, 0.08f, 0.12f, 0.98f});
    text(renderer, "LOJA DE MELHORIAS", 304, 138, 3);
    text(renderer, "OURO TOTAL " + std::to_string(combat.gold), 112, 168);
    const char *names[] = {"1 ALCANCE", "2 VIDAS", "3 OURO"};
    const char *descriptions[] = {"12 PX POR NIVEL", "1 VIDA POR NIVEL", "1 OURO POR MOEDA"};
    const char *units[] = {" PX", " VIDAS", " OURO"};
    for (int i = 0; i < 3; ++i)
    {
        const float x = 112.0f + i * 196.0f;
        const int price = progression.price(i);
        const bool available = price > 0 && combat.gold >= price;
        renderer.drawSolid({x, 196}, {180, 244}, {0.16f, 0.19f, 0.23f, 1});
        renderer.drawSolid({x, 196}, {180, 3}, available ? glm::vec4(0.3f, 0.8f, 0.4f, 1) : glm::vec4(0.5f, 0.5f, 0.5f, 1));
        text(renderer, names[i], x + 10, 208);
        renderer.draw(icons[i], {x + 58, 230}, {64, 64});
        text(renderer, descriptions[i], x + 10, 308);
        const int current = i == 0 ? 88 + 12 * progression.levels[i] : 1 + progression.levels[i];
        const int next = current + (i == 0 ? 12 : 1);
        text(renderer, "ATUAL " + std::to_string(current) + units[i], x + 10, 330);
        text(renderer, price ? "PROXIMO " + std::to_string(next) + units[i] : "NIVEL MAXIMO", x + 10, 352);
        text(renderer, "NIVEL " + std::to_string(progression.levels[i]) + "/5", x + 10, 374);
        text(renderer, price ? "PRECO " + std::to_string(price) + " OURO" : "COMPLETO", x + 10, 396);
        text(renderer, price ? (available ? "DISPONIVEL" : "OURO INSUFICIENTE") : "SEM NOVAS COMPRAS", x + 10, 418);
    }
    std::string uppercase = message;
    for (char &c : uppercase) if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
    text(renderer, uppercase, 112, 456);
    text(renderer, "MELHORIAS VALEM NA PROXIMA CORRIDA", 112, 478);
}
