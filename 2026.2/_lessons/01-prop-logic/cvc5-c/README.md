---
layout: page
title: cvc5 em C — instruções
nav_exclude: true
---

# Usando a API em C do cvc5

Este diretório contém os exemplos da aula escritos com a [API em C do
cvc5](https://cvc5.github.io/docs/cvc5-1.3.4/api/c/c.html):

| Arquivo          | O que faz                                                   |
|------------------|-------------------------------------------------------------|
| `sat.c`          | enumera todas as soluções de `(p ∨ q ∨ r) ∧ (¬p ∨ ¬q ∨ ¬r)` |
| `equivalencia.c` | testa se `¬(p ∨ (¬p ∧ q))` e `¬p ∧ ¬q` são equivalentes     |
| `n-rainhas.c`    | resolve o problema das `n` rainhas                          |
| `Makefile`       | compila os três programas                                   |

## 1. Instalando o cvc5

Diferentemente do Python, **não basta um `pip install`**: para programar em C
precisamos dos *cabeçalhos* (`cvc5/c/cvc5.h`) e da *biblioteca*
(`libcvc5.so` ou `libcvc5.a`).

### Opção A (recomendada): baixar um release pronto

Vá até a [página de releases do
cvc5](https://github.com/cvc5/cvc5/releases/latest) e baixe o arquivo
correspondente ao seu sistema. Por exemplo, no Linux x86-64:

Baixe e descompacte **dentro deste diretório**, junto dos programas `.c`:

``` shell
wget https://github.com/cvc5/cvc5/releases/download/cvc5-1.3.4/cvc5-Linux-x86_64-static.zip
unzip cvc5-Linux-x86_64-static.zip
```

Não é preciso mover nada de lugar: o `Makefile` encontra o cvc5 sozinho. O
diretório deve ficar assim:

```
cvc5-c/
├── cvc5-Linux-x86_64-static/
│   ├── include/cvc5/c/cvc5.h    <- cabeçalhos do cvc5
│   ├── lib/libcvc5.a            <- biblioteca do cvc5
│   └── bin/cvc5                 <- o executável de linha de comando
├── sat.c
├── equivalencia.c
├── n-rainhas.c
└── Makefile
```

Se preferir, você também pode copiar o `include/` e o `lib/` para cá e apagar
o resto — o `Makefile` aceita os dois arranjos.

Nos outros sistemas os nomes dos arquivos são análogos:

- macOS com chip Apple: `cvc5-macOS-arm64-static.zip`
- macOS com chip Intel: `cvc5-macOS-x86_64-static.zip`
- Windows: `cvc5-Win64-x86_64-static.zip` (use o WSL, ou o MSYS2/MinGW)

Os pacotes `-static` já trazem tudo embutido e são os mais simples de usar. Os
pacotes `-shared` também funcionam (veja a seção 2).

### Opção B: compilar a partir do código-fonte

``` shell
git clone https://github.com/cvc5/cvc5
cd cvc5
./configure.sh --auto-download --prefix=$HOME/cvc5-install
cd build && make -j$(nproc) && make install
```

Depois copie para cá o `include/` e o `lib/` gerados, como na opção A, ou
compile com `make CVC5_HOME=$HOME/cvc5-install` (veja a seção 2).

## 2. Compilando os exemplos

O `Makefile` deste diretório **assume que o cvc5 está aqui mesmo**, junto dos
programas. Então, tendo feito o passo anterior, basta:

``` shell
make
```

Isso gera os executáveis `sat`, `equivalencia` e `n-rainhas`. O `Makefile`
procura o cvc5 neste diretório — seja no release descompactado como veio
(`cvc5-.../include/` e `cvc5-.../lib/`), seja num `include/` + `lib/` copiados
para cá, seja com os arquivos soltos — e descobre sozinho se a biblioteca que
você tem é a estática ou a dinâmica.

Para conferir o que ele encontrou, use:

``` shell
make info
```

Se você preferir deixar o cvc5 em outro lugar, aponte `CVC5_HOME` para ele:

``` shell
make CVC5_HOME=$HOME/cvc5-Linux-x86_64-static
```

E se o cvc5 já estiver instalado num lugar padrão do sistema, use
`make CVC5_HOME=/usr/local` (ou simplesmente `make`, já que o compilador
procura nesses lugares por conta própria).

Se preferir chamar o compilador na mão, o comando equivalente — com o release
descompactado neste diretório — é, para os pacotes `-static`:

``` shell
CVC5=cvc5-Linux-x86_64-static
gcc -std=c11 -I$CVC5/include n-rainhas.c -o n-rainhas \
    -L$CVC5/lib -lcvc5 -lcadical -lpicpoly -lpicpolyxx -lgmp -lstdc++ -lm
```

e para os pacotes `-shared`:

``` shell
CVC5=cvc5-Linux-x86_64-shared
gcc -std=c11 -I$CVC5/include n-rainhas.c -o n-rainhas \
    -L$CVC5/lib -lcvc5 -Wl,-rpath,'$ORIGIN'/$CVC5/lib
```

O `-Wl,-rpath,...` grava no executável o caminho da biblioteca *relativo a ele
próprio* (`$ORIGIN` é o diretório do executável), para não ser preciso ajustar
a variável `LD_LIBRARY_PATH` a cada execução. As aspas simples em volta de
`$ORIGIN` são importantes: sem elas o shell tentaria expandi-lo.

Repare que, embora a API seja em C, a biblioteca por baixo é escrita em C++;
por isso o `-lstdc++` na ligação estática.

## 3. Executando

``` shell
./sat
./equivalencia
./n-rainhas
```

O `n-rainhas` imprime as 92 soluções do tabuleiro 8×8. Para mudar o tamanho do
tabuleiro, altere o `#define N 8` no início do arquivo e recompile.

## Problemas comuns

Antes de mais nada, rode `make info`: ele diz qual cabeçalho e qual biblioteca
o `Makefile` encontrou. Se aparecer `(nao encontrado aqui)`, o cvc5 não está
neste diretório e é isso que precisa ser resolvido.

- **`fatal error: cvc5/c/cvc5.h: No such file or directory`** — os cabeçalhos
  não foram encontrados. Confira se existe um
  `cvc5-*/include/cvc5/c/cvc5.h` (ou `include/cvc5/c/cvc5.h`) neste diretório.
  Um engano comum é descompactar o release em outro lugar, ou descompactar o
  pacote errado (o `.jar` do Java, por exemplo).
- **`cannot find -lcvc5`** — os cabeçalhos foram encontrados mas a biblioteca
  não. Veja se o `lib/` do release veio junto.
- **`error while loading shared libraries: libcvc5.so.1`** — o executável não
  achou a biblioteca dinâmica em tempo de execução. Isso não deve acontecer
  usando o `Makefile` (ele grava o caminho no executável com `-rpath`); se
  você compilou na mão, ou recompile com o `-Wl,-rpath,...`, ou rode com
  `LD_LIBRARY_PATH=<dir do lib> ./n-rainhas`. Note também que mover o
  executável para outro diretório sem levar a biblioteca junto quebra o
  `$ORIGIN`.
- **`undefined reference to 'std::...'`** — falta o `-lstdc++` no fim da linha
  de ligação (ou use `g++` para ligar). A API é em C, mas o cvc5 por baixo é
  C++.
- **Ordem das bibliotecas** — na ligação estática, o `gcc` é sensível à ordem:
  as bibliotecas vêm *depois* do arquivo `.c`, e as dependências depois de
  `-lcvc5`.
- **Baixei o release de outra arquitetura** — num Mac com chip Apple, o pacote
  `x86_64` compila mas dá erro de arquitetura na ligação; use o `arm64`.
