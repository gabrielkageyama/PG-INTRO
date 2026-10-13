# Trabalho Grau A - Endless Runner Medieval

## Equipe

- Gabriel Guerra Kageyama

---

## Descrição do Projeto

Este projeto foi desenvolvido para a disciplina **Processamento Gráfico:
Fundamentos** com o objetivo de aplicar os conteúdos estudados em OpenGL moderno.
O resultado é um endless runner medieval 2D no qual um cavaleiro percorre três
pistas, desvia de obstáculos e derrota inimigos com um golpe de espada.

Existem três tipos de inimigo: o orc, sem habilidade especial; o mago, que lança
uma bola de fogo na própria pista; e o ladino, que sinaliza e ataca uma pista
adjacente. Inimigos derrotados deixam moedas. Depois do game over, o ouro pode ser
usado para comprar melhorias de alcance, vidas e valor das moedas. A velocidade
das ameaças aumenta durante a corrida.

O trabalho explora shaders, VAO, VBO, EBO, projeção ortográfica, model matrices,
texturas, spritesheets, animação por tempo, colisões AABB, camadas com parallax
scrolling e entrada pelo teclado. Também foram implementados HUD, pausa, áudio,
menu, game over e loja.

---

## Estrutura do Projeto

| Arquivo | Descrição |
| --- | --- |
| `main.cpp` | Cria a janela e o contexto OpenGL, configura callbacks e executa o loop principal. |
| `Game.h / Game.cpp` | Coordena estados, input, cenário, entidades, combate, HUD e áudio. |
| `Renderer2D.h / Renderer2D.cpp` | Mantém o quad compartilhado, buffers, shaders, matrizes e draw calls. |
| `Texture.h / Texture.cpp` | Carrega imagens RGBA com `stb_image` e gerencia texturas OpenGL. |
| `Sprite.h` | Armazena e atualiza os dados de animação das spritesheets. |
| `Player.h / Player.cpp` | Controla pista, corrida, ataque, janela ativa e cooldown do jogador. |
| `Encounters.h / Encounters.cpp` | Gera e movimenta inimigos, obstáculos, projéteis e ondas. |
| `Collision.h` | Define as hitboxes e o teste de colisão AABB. |
| `Combat.h / Combat.cpp` | Resolve ataques, dano, invulnerabilidade, moedas e distância. |
| `World.h / World.cpp` | Desenha as camadas do cenário, tiles e parallax. |
| `ScrollLayer.h` | Calcula deslocamento, velocidade relativa e repetição das camadas. |
| `Progression.h` | Controla ouro, níveis, preços, compras e melhorias. |
| `Hud.h / Hud.cpp` | Desenha vidas, contadores, recarga, controles e loja. |
| `Audio.h / Audio.cpp` | Reproduz música e efeitos com miniaudio. |
| `Config.h / Config.cpp` | Lê e valida as opções de balanceamento de `game.cfg`. |
| `RenderCheck.h / RenderCheck.cpp` | Executa as verificações gráficas do smoke test. |
| `tests/` | Testes automatizados da lógica do jogador, mundo, encontros, combate, progressão, configuração e áudio. |
| `CMakeLists.txt` | Configuração independente de compilação e dependências do jogo. |

Os arquivos finais de imagem, áudio, configuração e créditos ficam em
`assets/TrabalhoGrauA`, na raiz do repositório. As bibliotecas incluídas
diretamente no projeto ficam em `third_party`.

---

## Informações Técnicas

- **Linguagem:** C++17
- **API gráfica:** OpenGL 3.3 Core
- **Shaders:** GLSL 330 Core
- **Janela e input:** GLFW 3.4
- **Carregamento OpenGL:** GLAD
- **Matemática:** GLM 1.0.1
- **Imagens:** stb_image
- **Áudio:** miniaudio 0.11.23 e stb_vorbis
- **Build:** CMake 3.14 ou superior
- **Plataforma validada:** Windows com MinGW-UCRT64
- **Plataformas compatíveis previstas:** Linux e macOS, ainda não testados neste projeto

### Compilação e execução

Execute os comandos a partir da raiz do repositório.

#### Windows com MinGW

```sh
cmake -S src/Trabalhos/TrabalhoGrauA -B build-grau-a -G "MinGW Makefiles"
cmake --build build-grau-a
./build-grau-a/TrabalhoGrauA.exe
```

#### Linux ou macOS

```sh
cmake -S src/Trabalhos/TrabalhoGrauA -B build-grau-a
cmake --build build-grau-a
./build-grau-a/TrabalhoGrauA
```

São necessários CMake, Git, compilador C/C++17 e suporte a OpenGL 3.3 Core.
A primeira configuração utiliza internet para obter GLFW e GLM. No Windows, o
MinGW-UCRT64 deve estar no `PATH`. No Linux, também são necessários os pacotes de
desenvolvimento do OpenGL e dos backends utilizados pela GLFW.

Também é possível compilar o repositório inteiro:

```sh
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/trabalho-grau-a/TrabalhoGrauA.exe
```

No Linux e no macOS, omita o gerador MinGW e a extensão `.exe`.

### Controles

| Tecla | Ação |
| --- | --- |
| `Enter` | Inicia ou reinicia a corrida |
| `W` ou seta para cima | Move para a pista superior |
| `S` ou seta para baixo | Move para a pista inferior |
| `Espaço` | Executa o golpe de espada |
| `P` | Pausa ou continua a corrida |
| `H` | Mostra ou oculta as hitboxes |
| `L` | Abre a loja depois do game over |
| `1`, `2`, `3` | Compra uma melhoria na loja |
| `9` / `0` | Diminui ou aumenta o volume |
| `M` | Ativa ou desativa o som |
| `Esc` | Encerra a aplicação |

### Resumo da implementação

O renderer utiliza um quad unitário compartilhado. O VBO contém quatro vértices
com posição XY e coordenadas UV; o EBO possui seis índices que formam dois
triângulos; e o VAO registra os dois atributos. Cada objeto é desenhado com uma
model matrix própria, aplicando translação, rotação e escala. A projeção
`glm::ortho(0, 800, 600, 0, -1, 1)` cria um espaço 2D com origem no canto superior
esquerdo.

O vertex shader calcula `projection * model * position` e seleciona o trecho da
spritesheet com `uvOffset` e `uvScale`. O fragment shader amostra a textura ou
usa uma cor uniforme para elementos da HUD, avisos e hitboxes. As animações são
controladas por tempo decorrido. Os filtros `GL_NEAREST` preservam a pixel art e
o alpha blending permite transparência.

As colisões usam retângulos AABB menores que as imagens, ignorando margens
transparentes. A tecla `H` desenha as mesmas caixas utilizadas pela lógica. O
cenário possui três velocidades: vila distante com fator 0,20, árvores próximas
com 0,45 e chão com 1,0, produzindo o efeito de parallax. Todas as movimentações,
animações e temporizações utilizam delta time.

O arquivo `assets/TrabalhoGrauA/config/game.cfg` permite ajustar velocidades,
intervalos, ataque e preços sem recompilar. Valores inválidos são ignorados e os
valores padrão são preservados.

---

## Checklist de Requisitos

- [x] Janela e contexto OpenGL 3.3 Core ou superior
- [x] Vertex shader e fragment shader próprios
- [x] Criação e utilização de VAO, VBO e EBO
- [x] Vértices com posição e coordenadas de textura
- [x] Projeção ortográfica e model matrix por objeto
- [x] Sprites aplicados como texturas em polígonos
- [x] Personagem, inimigos, itens e efeitos animados com spritesheets
- [x] Controle do personagem por teclado
- [x] Colisões entre jogador, inimigos, obstáculos, projéteis, ataque e moedas
- [x] Cenário com múltiplas camadas e parallax scrolling
- [x] Arquivo externo de configuração
- [x] Dificuldade progressiva durante a corrida
- [x] Três tipos de inimigo com comportamentos diferentes
- [x] Ouro e melhorias de alcance, vidas e recompensa
- [x] HUD, menu, pausa, game over e loja
- [x] Música e efeitos sonoros
- [x] Build independente e integrado ao repositório

## Vídeo de Apresentação

- **Vídeo:** link a adicionar após a gravação

---

## Referências e/ou Créditos

- [Exemplos da disciplina - PG2026-2](https://github.com/fellowsheep/PG2026-2)
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [stb](https://github.com/nothings/stb)
- [miniaudio](https://github.com/mackron/miniaudio)
- [LearnOpenGL](https://learnopengl.com/)
- [Origens e licenças dos assets](../../../assets/TrabalhoGrauA/licenses/ATTRIBUTION.md)

Os avisos e licenças das bibliotecas incluídas diretamente no projeto estão em
`third_party`. Os créditos específicos das imagens e dos arquivos de áudio estão
na pasta de licenças dos assets.

---

## Comentários Finais

O jogo mantém a lógica principal separada das chamadas OpenGL, facilitando a
leitura. O personagem alternativo paladino não foi implementado. O saldo e as
melhorias permanecem somente durante a execução: ao fechar o programa,
a progressão é reiniciada. Se o dispositivo ou algum arquivo de áudio não estiver
disponível, o jogo continua funcionando silenciosamente.

A compilação e a execução foram verificadas no Windows. As instruções para Linux
e macOS foram incluídas, mas ainda precisam ser validadas nesses sistemas.
