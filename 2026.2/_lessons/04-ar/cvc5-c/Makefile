# Compila os exemplos da API em C do cvc5.
#
# O cvc5 é procurado NESTE MESMO DIRETÓRIO, junto dos programas. Basta baixar
# o release do GitHub aqui, descompactar e rodar:
#
#   unzip cvc5-Linux-x86_64-static.zip
#   make
#
# Funciona tanto com o release descompactado como veio (um diretório
# `cvc5-.../` aqui dentro) quanto com o `include/` e o `lib/` dele copiados
# direto para cá.
#
# Se você preferir manter o cvc5 em outro lugar, ou se ele já estiver
# instalado no sistema, aponte CVC5_HOME para lá:
#
#   make CVC5_HOME=$HOME/cvc5-Linux-x86_64-static
#   make CVC5_HOME=/usr/local
#
# Veja o README.md para mais detalhes.

# ---------------------------------------------------------------------------
# Onde está o cvc5
# ---------------------------------------------------------------------------

# Procuramos o cabeçalho principal neste diretório, primeiro em `include/` e
# depois dentro de um release descompactado (`cvc5-.../include/`). O
# diretório encontrado vira o CVC5_HOME.
CVC5_HEADER := $(firstword \
    $(wildcard ./include/cvc5/c/cvc5.h ./cvc5*/include/cvc5/c/cvc5.h))
CVC5_FOUND := $(patsubst %/include/cvc5/c/cvc5.h,%,$(CVC5_HEADER))
CVC5_HOME ?= $(if $(CVC5_FOUND),$(CVC5_FOUND),.)

# ---------------------------------------------------------------------------
# Opções de compilação
# ---------------------------------------------------------------------------

CFLAGS  = -std=c11 -Wall -O2
CFLAGS += -I$(CVC5_HOME)/include -I$(CVC5_HOME)

LDFLAGS = -L$(CVC5_HOME)/lib -L$(CVC5_HOME)

# Para achar o libcvc5.so em tempo de execução, gravamos o caminho da
# biblioteca dentro do executável, evitando ter que mexer no LD_LIBRARY_PATH.
# Se o caminho for relativo, usamos $ORIGIN, que o carregador substitui pelo
# diretório do próprio executável.
ifeq ($(patsubst /%,,$(CVC5_HOME)),)
  LDFLAGS += -Wl,-rpath,$(CVC5_HOME)/lib -Wl,-rpath,$(CVC5_HOME)
else
  LDFLAGS += -Wl,-rpath,'$$ORIGIN/$(CVC5_HOME)/lib' \
             -Wl,-rpath,'$$ORIGIN/$(CVC5_HOME)'
endif

LDLIBS = -lcvc5

# Se só houver a biblioteca estática (libcvc5.a), precisamos listar também as
# dependências dela na hora da ligação. Note o -lstdc++: a API é em C, mas o
# cvc5 por baixo é escrito em C++.
LIBCVC5_SO := $(wildcard $(CVC5_HOME)/lib/libcvc5.so $(CVC5_HOME)/libcvc5.so)
LIBCVC5_A  := $(wildcard $(CVC5_HOME)/lib/libcvc5.a $(CVC5_HOME)/libcvc5.a)
ifeq ($(LIBCVC5_SO),)
  ifneq ($(LIBCVC5_A),)
    LDLIBS += -lcadical -lpicpoly -lpicpolyxx -lgmp -lstdc++ -lm
  endif
endif

# ---------------------------------------------------------------------------

PROGRAMAS = sat equivalencia n-rainhas

all: $(PROGRAMAS)

clean:
	rm -f $(PROGRAMAS)

# Mostra onde o cvc5 foi encontrado (útil para depurar a instalação).
info:
	@echo "CVC5_HOME = $(CVC5_HOME)"
	@echo "cabeçalho = $(if $(CVC5_HEADER),$(CVC5_HEADER),(nao encontrado aqui; usando os caminhos padrao do sistema))"
	@echo "biblioteca= $(if $(LIBCVC5_SO),$(LIBCVC5_SO),$(if $(LIBCVC5_A),$(LIBCVC5_A),(nao encontrada aqui)))"

.PHONY: all clean info
