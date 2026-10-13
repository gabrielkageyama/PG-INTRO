# Origens e licenças dos assets

As origens foram mantidas para transparência acadêmica e rastreabilidade. A seleção inclui materiais CC0 e o pacote gratuito de personagens indicado abaixo.

| Conteúdo | Autor | Licença | Origem |
|---|---|---|---|
| Tiny Dungeon | Kenney | CC0 1.0 | https://kenney.nl/assets/tiny-dungeon |
| Tiny Town | Kenney | CC0 1.0 | https://kenney.nl/assets/tiny-town |
| Free Medieval 16x16 Sprite Pack | LeoTheLionKnight | gratuito para projetos comerciais e não comerciais; modificação permitida; redistribuição isolada proibida | https://leothelionknight.itch.io/free-medieval-16x16-sprite-pack |
| CC0 Currency | ZaninDevelopers | CC0 | https://opengameart.org/content/cc0-currency |
| Hearts 32x32 e 16x16 | Leozlk | CC0 | https://opengameart.org/content/hearts-32x32-16x16 |
| Swishes Sound Pack | artisticdude | CC0 | https://opengameart.org/content/swishes-sound-pack |
| 80 CC0 RPG SFX | rubberduck | CC0 | https://opengameart.org/node/86018 |
| Chiptune Battle Music | pmiller | CC0 | https://opengameart.org/content/chiptune-battle-music |
| Game Over Sound (Old School) | den_yes | CC0 | https://opengameart.org/content/game-over-soundold-school |

As cópias das licenças da Kenney estão nos arquivos `kenney-tiny-dungeon-license.txt` e `kenney-tiny-town-license.txt`.

As antigas planilhas de Eldiran foram deslocadas para `_sources/legacy-character-sheets/` apenas como histórico e não fazem parte da seleção visual atual.

## Asset criado especificamente para este projeto

Os seis obstáculos em `obstacles/medieval-obstacles-6x32.png` foram desenhados diretamente pelo script do projeto em uma grade de 16 x 16 pixels, usando formas e uma paleta limitada. Eles não são derivados das imagens geradas anteriormente.

`effects/sword-slash-6x64.png` foi derivado de uma imagem criada com o gerador de imagens da OpenAI e depois convertido localmente em seis quadros de 64 x 64 px com transparência.

Prompt utilizado:

> Use case: stylized-concept. Asset type: 2D game effect sprite sheet. Primary request: a six-frame pixel-art sword slash effect animation for a top-down medieval game, showing one clean frontal semicircular slash progressing from anticipation to bright active arc to fading recovery. Style/medium: crisp 16-bit pixel art, limited pale blue, white, and light cyan palette. Composition/framing: exactly six equal square frames in one horizontal row, consistent 64x64 frame canvas, effect centered and never crossing frame boundaries. Scene/backdrop: transparent background. Constraints: genuinely transparent background; no character; no weapon handle; no text; no grid lines; no shadows; no watermark; each frame clearly separated by transparent space; nearest-neighbor pixel edges. Avoid: painterly glow, gradients, blur, anti-aliased edges, perspective scene, extra objects.

O resultado original está preservado em `_sources/generated-sword-slash-original.png`; o arquivo de uso no jogo é a versão recortada e padronizada.
