# Lista de Exercícios 2 — Projeção Ortográfica e Viewport

## Equipe

- Trabalho individual — Gabriel Guerra Kageyama.

## Descrição do projeto

Esta lista explora o mapeamento entre coordenadas do mundo e a janela da aplicação. Os seis programas utilizam projeção ortográfica, diferentes configurações de viewport e entrada do mouse para posicionar e criar triângulos em uma cena 2D.

## Exercícios

| Exercício | Descrição |
| --- | --- |
| `Ex1/main.cpp` | Usa uma janela de mundo com X e Y entre -10 e 10. |
| `Ex2/main.cpp` | Configura uma câmera 2D de 800 × 600 com origem no canto superior esquerdo. |
| `Ex3/main.cpp` | Posiciona uma cena diretamente em unidades correspondentes à janela. |
| `Ex4/main.cpp` | Desenha a cena somente no quadrante superior direito. |
| `Ex5/main.cpp` | Repete a mesma cena nos quatro quadrantes da janela. |
| `Ex6/main.cpp` | Cria um vértice por clique e um triângulo colorido a cada três cliques. |

Os arquivos `Common/OpenGLUtils.h` e `Common/OpenGLUtils.cpp` concentram as operações compartilhadas: criação da janela, carregamento da GLAD, configuração inicial da viewport, compilação dos shaders e tratamento da tecla `Esc`.

## Informações técnicas

- **Linguagem:** C++17
- **API gráfica:** OpenGL 3.3 Core
- **Bibliotecas:** GLFW, GLAD e GLM
- **Construção:** CMake 3.14 ou superior
- **Plataforma principal:** Windows com MinGW-UCRT64, conforme o ambiente indicado na disciplina

## Como compilar

Na raiz do repositório:

### Windows com MSYS2/MinGW

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

### Linux ou macOS

```bash
cmake -S . -B build
cmake --build build
```

No Windows, o MinGW-UCRT64 deve estar instalado e disponível no `PATH`, conforme o `GettingStarted.md` da disciplina. No Linux e no macOS, o CMake seleciona o gerador disponível no ambiente.

Para executar no Windows:

```bash
./build/Lista2_Ex1.exe
./build/Lista2_Ex2.exe
./build/Lista2_Ex3.exe
./build/Lista2_Ex4.exe
./build/Lista2_Ex5.exe
./build/Lista2_Ex6.exe
```

## Controles

### Exercícios 1 a 5

| Tecla | Resultado |
| --- | --- |
| `Esc` | Encerra o programa |

### Exercício 6

| Entrada | Resultado |
| --- | --- |
| Clique esquerdo | Adiciona um vértice na posição do cursor |
| Três cliques | Cria um novo triângulo colorido |
| `C` | Remove os triângulos e vértices pendentes |
| `Esc` | Encerra o programa |

## Resposta do exercício 3

Com a projeção `ortho(0, 800, 600, 0)`, os objetos são posicionados em unidades que correspondem diretamente às dimensões da janela. A coordenada `(0, 0)` fica no canto superior esquerdo, X cresce para a direita e Y cresce para baixo.

Essa configuração é útil para aplicações 2D porque aproxima as coordenadas do mundo das coordenadas da tela e do mouse. Assim, um objeto colocado em `(400, 300)` aparece no centro de uma janela 800 × 600, reduzindo a necessidade de conversões manuais para posicionar interfaces, sprites e formas selecionadas pelo cursor.

## Resumo técnico geral

O projeto aplica os conteúdos de sistemas de coordenadas e transformações vistos em aula. A GLM cria as matrizes de projeção ortográfica, que são enviadas ao vertex shader por uma uniform. Os exercícios também mostram que a viewport determina a região do framebuffer usada para transformar as coordenadas normalizadas em pixels da janela.

No exercício interativo, o callback de mouse da GLFW registra cada clique e suas coordenadas são mapeadas para o mundo 800 × 600. Um único VAO descreve os atributos de posição e cor, enquanto um VBO com uso `GL_DYNAMIC_DRAW` é atualizado quando os pontos mudam. Os vértices ainda incompletos aparecem como pontos brancos e cada grupo de três passa a ser desenhado como um triângulo de nova cor.

## Checklist de requisitos

- [x] Janela de mundo com `xmin=-10`, `xmax=10`, `ymin=-10` e `ymax=10`
- [x] Câmera 2D com `xmin=0`, `xmax=800`, `ymin=600` e `ymax=0`
- [x] Cena posicionada diretamente em coordenadas da janela
- [x] Explicação sobre a utilidade da câmera 2D
- [x] Cena desenhada somente no quadrante superior direito
- [x] Mesma cena desenhada nos quatro quadrantes
- [x] Um vértice criado por clique do mouse
- [x] Um triângulo criado a cada três vértices
- [x] Nova cor atribuída a cada triângulo

## Referências e créditos

- [Projeto de exemplos PG2026-2](https://github.com/fellowsheep/PG2026-2)
- [LearnOpenGL — Coordinate Systems](https://learnopengl.com/Getting-started/Coordinate-Systems)
- [LearnOpenGL — Transformations](https://learnopengl.com/Getting-started/Transformations)
- [GLFW — Input Guide](https://www.glfw.org/docs/latest/input_guide.html)
- Materiais de aula sobre sistemas de coordenadas, transformações, shaders, uniforms e buffers

## Comentários finais

A implementação prioriza as técnicas apresentadas nas aulas e mantém cada exercício em um programa pequeno, com a progressão proposta no enunciado.
