# Processamento Gráfico: Fundamentos 2026/2

Repositório com os exercícios desenvolvidos para a Atividade Acadêmica **Processamento Gráfico: Fundamentos**, do curso de Ciência da Computação da Unisinos.

**Aluno:** Gabriel Guerra Kageyama.

## Estrutura do repositório

| Projeto | Engine / Linguagem | Descrição breve |
| --- | --- | --- |
| [Lista 1](src/Exercicios/Lista1/README.md) | C++ / OpenGL | Primitivas gráficas, shaders, buffers e geração de geometria 2D |
| [Lista 2](src/Exercicios/Lista2/README.md) | C++ / OpenGL | Projeção ortográfica, viewport e entrada do mouse |
| [Trabalho Grau A](src/Trabalhos/TrabalhoGrauA/README.md) | C++ / OpenGL | Endless runner medieval — combate, loja, HUD, pausa e áudio |

```text
PG-INTRO/
├── Common/                         # GLAD e funções OpenGL compartilhadas
├── include/glad/                   # Cabeçalhos da GLAD
├── src/Exercicios/
│   ├── Lista1/                     # Quatro exercícios da primeira lista
│   └── Lista2/                     # Seis exercícios da segunda lista
├── src/Trabalhos/TrabalhoGrauA/     # Jogo com renderer, CMake e dependências próprios
├── assets/TrabalhoGrauA/            # Texturas, áudio, configuração e licenças do jogo
├── CMakeLists.txt
└── README.md
```

## Tecnologias

- C++17
- OpenGL 3.3 Core
- GLFW 3.4
- GLAD
- GLM 1.0.1
- CMake 3.14 ou superior

## Compilação

O projeto utiliza CMake e baixa a GLFW automaticamente durante a primeira configuração. É necessário ter CMake, Git e um compilador C/C++ instalados.

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

Os executáveis gerados podem ser encontrados no diretório `build`, criado durante a configuração.

O Trabalho Grau A tem CMake, dependências e build próprios, sem compartilhar
bibliotecas com os exercícios. O build da raiz apenas aciona esse projeto separado,
gerando `build/trabalho-grau-a/TrabalhoGrauA`. Para compilar somente o jogo:

```bash
cmake -S src/Trabalhos/TrabalhoGrauA -B build-grau-a -G "MinGW Makefiles"
cmake --build build-grau-a
./build-grau-a/TrabalhoGrauA.exe
```

No Linux/macOS, omita o gerador MinGW e a extensão `.exe`. Consulte o README do
trabalho para os requisitos e a organização da entrega independente.

### Se o comando `cmake` não for reconhecido

O CMake é uma ferramenta externa. Instale-o conforme o guia `GettingStarted.md` da disciplina, habilite a opção de adicioná-lo ao `PATH` e reinicie sua IDE/terminal para carregar a nova configuração. Confirme com `cmake --version` antes de executar os comandos acima. No Windows, também é necessário ter o MinGW-UCRT64 indicado pela professora. No Linux e no macOS, utilize um compilador C/C++ e uma ferramenta de construção compatíveis com o gerador escolhido automaticamente pelo CMake.

## Observações

- O repositório continuará público durante o período de avaliação da disciplina.
- A pasta `build` não é versionada.
- A estrutura e a configuração foram baseadas no projeto de exemplos da disciplina.

## Referências

- [Repositório de exemplos PG2026-2](https://github.com/fellowsheep/PG2026-2)
- [LearnOpenGL](https://learnopengl.com/)
- Materiais disponibilizados nas aulas de Processamento Gráfico: Fundamentos com a Professora Rossana Baptista Queiroz
