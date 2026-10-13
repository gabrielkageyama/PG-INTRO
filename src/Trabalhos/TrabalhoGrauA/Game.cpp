#include "Game.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>

bool Game::initialize(const std::string &assets)
{
    config = loadConfig(assets + "/config/game.cfg");
    progression.basePrices = config.prices;
    if (!knight.load(assets + "/characters/player/knight-run-front-7x32.png") ||
        !slash.load(assets + "/effects/sword-slash-6x64.png"))
        return false;
    if (knight.width() != 224 || knight.height() != 32 ||
        slash.width() != 384 || slash.height() != 64)
    {
        std::cerr << "Dimensoes inesperadas: cavaleiro deve ser 224x32 e slash 384x64.\n";
        return false;
    }
    const char *enemyFiles[] = {"basic-orc-7x32.png", "projectile-mage-7x32.png", "adjacent-rogue-7x32.png"};
    for (int i = 0; i < 3; ++i)
    {
        if (!enemyTextures[i].load(assets + "/characters/enemies/" + enemyFiles[i])) return false;
        if (enemyTextures[i].width() != 224 || enemyTextures[i].height() != 32)
        {
            std::cerr << "Spritesheet de inimigo deve ser 224x32.\n";
            return false;
        }
    }
    if (!obstacleTexture.load(assets + "/obstacles/medieval-obstacles-6x32.png") ||
        !fireballTexture.load(assets + "/projectiles/fireball-left-4x32.png")) return false;
    if (obstacleTexture.width() != 192 || obstacleTexture.height() != 32 ||
        fireballTexture.width() != 128 || fireballTexture.height() != 32)
    {
        std::cerr << "Dimensoes inesperadas nos obstaculos/projeteis.\n";
        return false;
    }
    if (!coinTexture.load(assets + "/items/coin-spin-4x32.png") ||
        coinTexture.width() != 128 || coinTexture.height() != 32) return false;
    const char *shopFiles[] = {"shop-attack-range-64x64.png", "shop-extra-life-64x64.png", "shop-extra-gold-64x64.png"};
    for (int i = 0; i < 3; ++i)
        if (!shopTextures[i].load(assets + "/ui/" + shopFiles[i]) ||
            shopTextures[i].width() != 64 || shopTextures[i].height() != 64) return false;
    if (!world.initialize(assets) || !hud.initialize(assets)) return false;
    audio.initialize(assets); // Audio indisponivel nunca impede jogar.
    return true;
}

void Game::handleKey(int key, int action)
{
    // RELEASE e REPEAT nunca disparam movimento nem ataques.
    if (action != GLFW_PRESS)
        return;
    if (key == GLFW_KEY_H) showHitboxes = !showHitboxes;
    if (key == GLFW_KEY_M)
    {
        muted = !muted;
        audio.setVolume(getVolume());
        return;
    }
    if (key == GLFW_KEY_9 || key == GLFW_KEY_0)
    {
        volume = std::clamp(getVolume() + (key == GLFW_KEY_0 ? 10 : -10), 0, 100);
        muted = false;
        audio.setVolume(volume);
        return;
    }
    if (key == GLFW_KEY_P && state == GameState::Playing)
    {
        paused = !paused;
        audio.pause(paused);
        return;
    }
    if (paused) return;
    if (key == GLFW_KEY_ENTER && state != GameState::Playing)
    {
        startRun();
        return;
    }
    if (state == GameState::GameOver && key == GLFW_KEY_L)
    {
        state = GameState::Shop;
        shopMessage.clear();
        return;
    }
    if (state == GameState::Shop && key >= GLFW_KEY_1 && key <= GLFW_KEY_3)
    {
        const int item = key - GLFW_KEY_1;
        const bool purchased = progression.buy(item, combat.gold);
        if (purchased) audio.play(Sound::Purchase);
        shopMessage = purchased ? "Comprado!" :
            (progression.price(item) == 0 ? "Nivel maximo" : "Ouro insuficiente");
    }
    if (state != GameState::Playing) return;
    if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
        player.changeLane(-1);
    else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
        player.changeLane(1);
    else if (key == GLFW_KEY_SPACE)
    {
        if (player.tryAttack()) audio.play(Sound::Slash);
    }
}

void Game::update(double deltaTime)
{
    if (state != GameState::Playing || paused) return;
    // Passos pequenos impedem que movimento ou animacao pulem uma janela de acerto.
    while (deltaTime > 1e-9 && combat.alive())
    {
        const double step = std::min(deltaTime, 1.0 / 120.0);
        player.update(step);
        encounters.update(step);
        combat.update(step, player, encounters);
        if (encounters.firedThisStep) audio.play(Sound::Fireball);
        if (combat.enemyDefeated) audio.play(Sound::EnemyHit);
        if (combat.playerHurt) audio.play(combat.obstacleHurt ? Sound::Obstacle : Sound::PlayerHit);
        if (combat.collected) audio.play(Sound::Coin);
        world.update(step, encounters.speed());
        deltaTime -= step;
    }
    if (!combat.alive())
    {
        state = GameState::GameOver;
        audio.stop(Sound::Music);
        audio.play(Sound::GameOver);
    }
}

void Game::startRun()
{
    progression.startRun(player, combat);
    player.cooldownDuration = config.attackCooldown;
    player.slash.frameDuration = config.attackFrame;
    encounters = Encounters(std::random_device{}(), config);
    world.reset();
    shopMessage.clear();
    state = GameState::Playing;
    paused = false;
    audio.beginRun();
}

void Game::draw(const Renderer2D &renderer) const
{
    world.draw(renderer);
    drawEntities(renderer);
    const float runFrameWidth = 1.0f / player.run.frameCount;
    // Pisca somente enquanto vivo e protegido depois de um dano.
    if (!combat.alive() || static_cast<int>(combat.invulnerability * 10) % 2 == 0)
        renderer.draw(knight,
        {Player::centerX - Player::spriteSize / 2.0f, player.centerY() - Player::spriteSize / 2.0f},
        {Player::spriteSize, Player::spriteSize},
        {player.run.currentFrame * runFrameWidth, 0.0f}, {runFrameWidth, 1.0f});

    if (player.attacking)
    {
        const float slashFrameWidth = 1.0f / player.slash.frameCount;
        // O arco original aponta para cima; +90 graus, com Y para baixo, aponta a direita.
        // O efeito acompanha o jogador inclusive ao trocar de linha durante o golpe.
        // A escala local Y vira alcance horizontal depois da rotacao de 90 graus.
        renderer.draw(slash, {Player::centerX - 20.0f + player.attackBonus / 2,
            player.centerY() - 64.0f - player.attackBonus / 2},
            {128.0f, 128.0f + player.attackBonus}, {player.slash.currentFrame * slashFrameWidth, 0.0f},
            {slashFrameWidth, 1.0f}, glm::half_pi<float>());
    }
    if (showHitboxes) drawHitboxes(renderer);
    if (state == GameState::Shop)
        hud.drawShop(renderer, progression, combat, shopTextures, shopMessage);
    hud.draw(renderer, combat, 1 + progression.levels[1], player, paused, getVolume(), audio.ready());
    hud.drawControls(renderer, state, paused);
}

std::string Game::windowTitle() const
{
    if (state == GameState::Menu)
        return "Grau A | Enter: iniciar | W/S: linhas | Espaco: atacar | P: pausa | 9/0: volume | Esc: sair";
    if (paused) return "Grau A - PAUSADO | P: continuar | 9/0: volume | Esc: sair";
    if (state == GameState::Shop)
    {
        std::string title = "Loja | Ouro Total " + std::to_string(combat.gold);
        const char *names[] = {" | 1 Alcance: ", " | 2 Vida: ", " | 3 Ouro: "};
        for (int i = 0; i < 3; ++i)
            title += names[i] + (progression.price(i) ? std::to_string(progression.price(i)) : std::string("MAX"));
        return title + " | Enter: correr | " + shopMessage;
    }
    std::string title = "Grau A | Vidas " + std::to_string(combat.lives) +
        " | Ouro Coletado " + std::to_string(combat.runGold) +
        " | Ouro Total " + std::to_string(combat.gold) +
        " | Distancia " + std::to_string(combat.distanceMeters()) + " m";
    if (state == GameState::GameOver)
        return "Fim | L: loja | Enter: reiniciar | Ouro Total " + std::to_string(combat.gold) +
            " | Ouro Coletado " + std::to_string(combat.runGold) +
            " | Distancia " + std::to_string(combat.distanceMeters()) + " m";
    title += " | Espaco: ";
    if (player.attacking)
        title += player.attackActive() ? "golpe ativo" : "golpe";
    else if (player.cooldownRemaining > 1e-9)
        title += "recarga " + std::to_string(static_cast<int>(std::ceil(player.cooldownRemaining * 1000.0))) + "ms";
    else
        title += "atacar";
    title += " | velocidade " + std::to_string(static_cast<int>(encounters.speed()));
    title += showHitboxes ? " | H: caixas ligadas" : " | H: caixas desligadas";
    return title;
}

void Game::drawEntities(const Renderer2D &renderer) const
{
    for (const Obstacle &obstacle : encounters.obstacles)
        renderer.draw(obstacleTexture, {obstacle.x - 32.0f, Player::laneCenters[obstacle.lane] - 24.0f},
            {64.0f, 64.0f}, {obstacle.variant / 6.0f, 0.0f}, {1.0f / 6.0f, 1.0f});
    for (const Coin &coin : combat.coins)
        renderer.draw(coinTexture, {coin.x - 16, Player::laneCenters[coin.lane] - 8},
            {32, 32}, {coin.animation.currentFrame / 4.0f, 0}, {0.25f, 1});
    for (const Projectile &projectile : encounters.projectiles)
        renderer.draw(fireballTexture, {projectile.x - 32.0f, Player::laneCenters[projectile.lane] - 24.0f},
            {64.0f, 64.0f}, {projectile.animation.currentFrame / 4.0f, 0.0f}, {0.25f, 1.0f});

    // Aviso amarelo, depois zona ativa vermelha. Reutiliza o mesmo quad do renderer.
    for (const Enemy &enemy : encounters.enemies)
    {
        if (enemy.type != EnemyType::Rogue ||
            (enemy.phase != RoguePhase::Warning && enemy.phase != RoguePhase::Active)) continue;
        const glm::vec2 position(enemy.x - 48.0f, Player::laneCenters[enemy.targetLane] - 8.0f);
        const bool active = enemy.phase == RoguePhase::Active;
        const glm::vec4 color = active ? glm::vec4(1.0f, 0.15f, 0.05f, 0.55f)
                                      : glm::vec4(1.0f, 0.85f, 0.1f, 0.40f);
        renderer.drawSolid(position, {96.0f, 48.0f}, color);
        const glm::vec4 border(color.r, color.g, color.b, 1.0f);
        renderer.drawSolid(position, {96.0f, 3.0f}, border);
        renderer.drawSolid(position + glm::vec2(0.0f, 45.0f), {96.0f, 3.0f}, border);
        renderer.drawSolid(position, {3.0f, 48.0f}, border);
        renderer.drawSolid(position + glm::vec2(93.0f, 0.0f), {3.0f, 48.0f}, border);
    }
    for (const Enemy &enemy : encounters.enemies)
        // Comeca na borda direita do quadro e percorre U ao contrario.
        renderer.draw(enemyTextures[static_cast<int>(enemy.type)],
            {enemy.x - 48.0f, Player::laneCenters[enemy.lane] - 48.0f}, {96.0f, 96.0f},
            {(enemy.animation.currentFrame + 1) / 7.0f, 0.0f}, {-1.0f / 7.0f, 1.0f});
}

void Game::drawHitboxes(const Renderer2D &renderer) const
{
    const auto outline = [&](Rect box, glm::vec4 color)
    {
        renderer.drawSolid({box.x, box.y}, {box.width, 2}, color);
        renderer.drawSolid({box.x, box.y + box.height - 2}, {box.width, 2}, color);
        renderer.drawSolid({box.x, box.y}, {2, box.height}, color);
        renderer.drawSolid({box.x + box.width - 2, box.y}, {2, box.height}, color);
    };
    outline(playerBox(player), {0, 1, 1, 1});
    if (player.attackActive()) outline(attackBox(player), {0, 1, 0, 1});
    for (const Enemy &enemy : encounters.enemies)
    {
        outline(enemyBox(enemy), {1, 0, 0, 1});
        if (enemy.type == EnemyType::Rogue && enemy.phase == RoguePhase::Active && enemy.targetLane >= 0)
            outline(rogueBox(enemy), {1, 0, 0, 1});
    }
    for (const Obstacle &obstacle : encounters.obstacles) outline(obstacleBox(obstacle), {1, 0.5f, 0, 1});
    for (const Projectile &projectile : encounters.projectiles) outline(projectileBox(projectile), {1, 0, 1, 1});
    for (const Coin &coin : combat.coins) outline(coinBox(coin), {1, 1, 0, 1});
}
