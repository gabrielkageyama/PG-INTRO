# Lista de Exercícios 1 — OpenGL Moderna

## Equipe

- Trabalho individual — Gabriel Guerra Kageyama.

## Descrição do projeto

Esta lista introduz a criação de aplicações gráficas 2D com OpenGL moderna e C++. Os quatro programas exploram primitivas gráficas, shaders, VBOs, VAOs, diferentes modos de desenho, geração paramétrica de vértices e composição de uma arte quadriculada.

## Exercícios

| Exercício | Descrição |
| --- | --- |
| `Ex1/main.cpp` | Desenha dois triângulos preenchidos, em contorno, como pontos ou com os três modos sobrepostos. |
| `Ex2/main.cpp` | Gera círculo, octógono, pentágono, pac-man, fatia de pizza, estrela e espiral com seno e cosseno. |
| `Ex3/main.cpp` | Desenha um triângulo com posição e cor RGB armazenadas por vértice. |
| `Ex4/main.cpp` | Converte uma matriz quadriculada em um coração 8-bit composto por triângulos. |

Os arquivos `Common/OpenGLUtils.h` e `Common/OpenGLUtils.cpp` concentram apenas operações repetidas: criação da janela, carregamento da GLAD, ajuste da viewport, compilação dos shaders e tratamento da tecla `Esc`.

## Informações técnicas

- **Linguagem:** C++17
- **API gráfica:** OpenGL 3.3 Core
- **Bibliotecas:** GLFW e GLAD
- **Construção:** CMake 3.14 ou superior
- **Plataforma principal:** Windows com MinGW-UCRT64, conforme o ambiente indicado na disciplina

## Como compilar

Na raiz do repositório:

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

No Windows, o MinGW-UCRT64 deve estar instalado e disponível no `PATH`, conforme o `GettingStarted.md` da disciplina. No Linux e no macOS, o CMake seleciona o gerador disponível no ambiente.

Para executar no Windows:

```sh
./build/Lista1_Ex1.exe
./build/Lista1_Ex2.exe
./build/Lista1_Ex3.exe
./build/Lista1_Ex4.exe
```

## Controles

### Exercício 1

| Tecla | Resultado |
| --- | --- |
| `1` | Triângulos preenchidos |
| `2` | Somente contornos |
| `3` | Somente pontos |
| `4` | Preenchimento, contorno e pontos juntos |
| `Esc` | Encerra o programa |

### Exercício 2

| Tecla | Forma |
| --- | --- |
| `1` | Círculo |
| `2` | Octógono |
| `3` | Pentágono |
| `4` | Pac-man |
| `5` | Fatia de pizza |
| `6` | Estrela |
| `7` | Espiral |
| `Esc` | Encerra o programa |

Nos exercícios 3 e 4, `Esc` encerra o programa.

## Resumo técnico geral

O projeto aplica os conteúdos introdutórios vistos em aula sobre OpenGL moderna: criação da janela e do loop de renderização, uso de vertex e fragment shaders, envio de dados por uniforms e buffers, configuração de VBOs e VAOs e desenho de primitivas. Os exercícios incluem diferentes representações de triângulos, formas geradas com a equação paramétrica do círculo, interpolação de cores por vértice e uma arte 8-bit construída a partir de quadrados formados por triângulos.

Como complemento à implementação básica apresentada nas aulas, os exercícios 1 e 2 permitem mudar a visualização pelas teclas numéricas, facilitando a demonstração de cada requisito em uma única janela. Também foi usado um callback de redimensionamento com viewport quadrada para preservar as proporções das formas, e o coração do exercício 4 foi descrito como uma matriz de caracteres para que o desenho possa ser alterado diretamente como uma grade de pixels.

## Checklist de requisitos

- [x] Dois triângulos preenchidos
- [x] Dois triângulos em contorno
- [x] Dois triângulos representados por pontos
- [x] Três modos de desenho sobrepostos
- [x] Círculo gerado parametricamente
- [x] Octógono e pentágono
- [x] Pac-man e fatia de pizza
- [x] Estrela e espiral
- [x] Posição e cor configuradas como atributos no VBO/VAO
- [x] Identificação dos atributos no vertex shader
- [x] Triângulo com cores interpoladas
- [x] Desenho quadriculado reproduzido com primitivas OpenGL
- [x] Uso de múltiplos VAOs e chamadas de desenho no exercício 4

## Referências e créditos

- [Projeto de exemplos PG2026-2](https://github.com/fellowsheep/PG2026-2)
- [LearnOpenGL — Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle)
- Materiais de aula sobre OpenGL moderna, shaders, buffers, sistemas de coordenadas e transformações
- A arte do exercício 4 foi inspirada na imagem de um coração 8-bit desenhada em papel quadriculado encontrado na internet.

## Comentários finais

A implementação prioriza as técnicas apresentadas nas aulas e mantém cada exercício em um programa pequeno.
