# Processamento Gráfico: Fundamentos 2026/2

Repositório com os exercícios desenvolvidos para a Atividade Acadêmica **Processamento Gráfico: Fundamentos**, do curso de Ciência da Computação da Unisinos.

**Aluno:** Gabriel Guerra Kageyama.

## Estrutura do repositório

| Projeto | Engine / Linguagem | Descrição breve |
| --- | --- | --- |
| [Lista 1](src/Exercicios/Lista1/README.md) | C++ / OpenGL | Primitivas gráficas, shaders, buffers e geração de geometria 2D |

```text
Lista01/
├── Common/                         # GLAD e funções OpenGL compartilhadas
├── include/glad/                   # Cabeçalhos da GLAD
├── src/Exercicios/Lista1/
│   ├── Ex1/                        # Dois triângulos e modos de desenho
│   ├── Ex2/                        # Formas geradas com seno e cosseno
│   ├── Ex3/                        # Posição e cor por vértice
│   ├── Ex4/                        # Coração em pixel art
│   └── README.md                   # Documentação da lista
├── CMakeLists.txt
└── README.md
```

## Tecnologias

- C++17
- OpenGL 3.3 Core
- GLFW 3.4
- GLAD
- CMake 3.14 ou superior

## Compilação

O projeto utiliza CMake e baixa a GLFW automaticamente durante a primeira configuração. É necessário ter CMake, Git e um compilador C/C++ instalados.

### Windows com MSYS2/MinGW

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

### Linux ou macOS

```bash
cmake -S . -B build
cmake --build build
```

Os executáveis gerados podem ser encontrados no diretório `build`, criado durante a configuração.

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
