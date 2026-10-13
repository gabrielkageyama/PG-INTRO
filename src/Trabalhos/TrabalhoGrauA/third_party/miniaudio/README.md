# Áudio exclusivo do jogo

- `miniaudio.h`: versão 0.11.23, de https://github.com/mackron/miniaudio/tree/0.11.23.
- SHA-256: `7e4f3f13c8fe66df2080ac3dd12a89193e3c2463cb7f067c798abd7331cd8ee6`.
- Licença MIT-0 ou domínio público, incluída em `LICENSE` e no cabeçalho.
- `../stb/stb_vorbis.c`: revisão `2c980bb59875b0d32144a71867fbdebb2f77cd20`,
  https://github.com/nothings/stb/blob/2c980bb59875b0d32144a71867fbdebb2f77cd20/stb_vorbis.c.
- SHA-256: `4c7cb2ff1f7011e9d67950446b7eb9ca044f2e464d76bfbb0b84dd2e23e65636`.
- Licença MIT ou domínio público, incluída no próprio arquivo e na pasta stb.

`implementation.c` inclui primeiro as declarações de stb_vorbis, depois a
implementação de miniaudio e finalmente a de stb_vorbis. Isso habilita OGG/Vorbis
além do WAV nativo. Cada implementação é compilada uma única vez como C.
Os arquivos de terceiros foram preservados; não dependem dos exercícios.
O CMake usa Threads e, no Unix, `m`/`${CMAKE_DL_LIBS}`. Não há download adicional
de áudio durante a configuração. `-DGRAU_A_AUDIO=OFF` remove essa integração.
