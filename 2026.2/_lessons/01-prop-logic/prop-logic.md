---
layout: page
title: Lógica proposicional
---

# Lógica proposicional
{: .no_toc .mb-2 }

- TOC
{:toc}

## Leituras

- Notas de aula:
  - [Introdução a Lógica Proposicional]({{ site.baseurl }}{% link _lessons/01-prop-logic/01-prop-logic-intro.pdf %})
  - [Aplicações e equivalências]({{ site.baseurl }}{% link _lessons/01-prop-logic/02-prop-logic-apps_equiv.pdf %})
  - [Satisfatibilidade]({{ site.baseurl }}{% link _lessons/01-prop-logic/03-prop-logic-sat.pdf %})
- Do livro texto:
  - Conectivos lógicos: capítulo 1.1
  - Aplicações, equivalências: capítulo 1.2

## Resolução de problemas via SAT

Podemos resolver problemas SAT automaticamente usando ferramentas de *automatização de raciocínio*. Um exemplo é o [cvc5](https://cvc5.github.io/), um solucionador SMT (de *satisfatibilidade módulo teorias*; falaremos um pouco mais sobre SMT em futuras aulas).

Como vimos em aula, podemos usar a [API](https://pt.wikipedia.org/wiki/Interface_de_programa%C3%A7%C3%A3o_de_aplica%C3%A7%C3%B5es) do cvc5 para escrever programas que resolvem problemas que podemos representar via SAT. O cvc5 oferece APIs para várias linguagens; aqui apresentamos cada exemplo **em Python e em C**. Os dois programas fazem exatamente a mesma coisa — o que muda é só a forma de construir as fórmulas.

### Preparando o ambiente em Python

Para instalar o `cvc5` e usar sua API em Python, basta instalar o cvc5 com, por exemplo:

``` shell
pip install cvc5
```

Mais instruções sobre instalação estão disponíveis [aqui](https://cvc5.github.io/docs/cvc5-1.3.4/api/python/python.html). E a documentação das operações disponíveis na API estão [aqui](https://cvc5.github.io/docs/cvc5-1.3.4/api/python/pythonic/pythonic.html).

### Preparando o ambiente em C

Em C não basta um `pip install`: é preciso baixar os cabeçalhos e a biblioteca
do cvc5 e passá-los ao compilador. As instruções completas (onde baixar, como
compilar, e os erros mais comuns) estão no
[README]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/README.md %}),
junto com os três programas prontos para compilar:
[`sat.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/sat.c %}),
[`equivalencia.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/equivalencia.c %}),
[`n-rainhas.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/n-rainhas.c %}) e um
[`Makefile`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/Makefile %}).

O resumo é: baixe um release pronto da [página de releases do
cvc5](https://github.com/cvc5/cvc5/releases/latest) e descompacte-o **no mesmo
diretório dos programas**. O `Makefile` procura o cvc5 ali e cuida do resto:

``` shell
wget https://github.com/cvc5/cvc5/releases/download/cvc5-1.3.4/cvc5-Linux-x86_64-static.zip
unzip cvc5-Linux-x86_64-static.zip
make
```

A documentação da API em C está
[aqui](https://cvc5.github.io/docs/cvc5-1.3.4/api/c/c.html). Vale conhecer suas
três ideias centrais, que aparecem em todos os programas abaixo:

- Um `Cvc5TermManager` constrói tipos e fórmulas; um `Cvc5*` é o solucionador,
  onde asserimos as fórmulas e fazemos as consultas. Ambos são criados no
  começo (`cvc5_term_manager_new`, `cvc5_new`) e destruídos no fim
  (`cvc5_delete`, `cvc5_term_manager_delete`).
- C não tem operadores sobrecarregados nem sintaxe de conveniência, então
  *toda* fórmula é construída com `cvc5_mk_term`, que recebe o conectivo
  (`CVC5_KIND_AND`, `CVC5_KIND_OR`, `CVC5_KIND_NOT`, `CVC5_KIND_IMPLIES`,
  `CVC5_KIND_EQUAL`, `CVC5_KIND_DISTINCT`, ...), o número de argumentos e um
  vetor com os argumentos.
- Uma variável proposicional é uma constante de tipo booleano, criada com
  `cvc5_mk_const(tm, cvc5_get_boolean_sort(tm), "p")`.

### Determinando a satisfatibilidade de uma fórmula proposicional

O programa abaixo, ao ser executado, apresnta todas as soluções para a fórmula

```
(p v q v r) ∧ (¬p v ¬q v ¬r)
```

**Em Python:**

```python
from cvc5.pythonic import *

if __name__ == '__main__':
    p, q, r = Bools("p q r")
    s = Solver()

    # p v q v r
    s.add(Or(p, q, r))
    # ¬p v ¬q v ¬r
    s.add(Or(Not(p), Not(q), Not(r)))

    count = 0
    while (s.check() == sat):
        m = s.model()
        print("Solution {}".format(count))
        print("p: ", m[p])
        print("q: ", m[q])
        print("r: ", m[r])
        s.add(Or(p != m[p], q != m[q], r != m[r]))
        print("============")
        count += 1
```

**Em C**, usando o arquivo [`sat.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/sat.c %}):

```c
/* Enumera todas as soluções da fórmula (p ∨ q ∨ r) ∧ (¬p ∨ ¬q ∨ ¬r)
 * usando a API em C do cvc5. */

#include <cvc5/c/cvc5.h>
#include <stdio.h>

int main(void)
{
  /* O gerenciador de termos (term manager) constrói tipos e fórmulas;
   * o objeto Cvc5 é o solucionador propriamente dito. */
  Cvc5TermManager* tm = cvc5_term_manager_new();
  Cvc5* solver = cvc5_new(tm);
  /* Precisamos dessa opção para poder consultar os valores das variáveis. */
  cvc5_set_option(solver, "produce-models", "true");

  /* Variáveis proposicionais são constantes de tipo booleano. */
  Cvc5Sort boolean = cvc5_get_boolean_sort(tm);
  Cvc5Term p = cvc5_mk_const(tm, boolean, "p");
  Cvc5Term q = cvc5_mk_const(tm, boolean, "q");
  Cvc5Term r = cvc5_mk_const(tm, boolean, "r");

  /* Em C não há operadores sobrecarregados: toda fórmula é construída com
   * cvc5_mk_term, que recebe o conectivo (kind), o número de argumentos e um
   * vetor com os argumentos. */

  /* p ∨ q ∨ r */
  Cvc5Term vars[3] = {p, q, r};
  cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_OR, 3, vars));

  /* ¬p ∨ ¬q ∨ ¬r */
  Cvc5Term negs[3];
  for (int i = 0; i < 3; i++)
    negs[i] = cvc5_mk_term(tm, CVC5_KIND_NOT, 1, &vars[i]);
  cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_OR, 3, negs));

  int count = 0;
  Cvc5Result res = cvc5_check_sat(solver);
  while (cvc5_result_is_sat(res))
  {
    Cvc5Term block[3];
    printf("Solution %d\n", count);
    for (int i = 0; i < 3; i++)
    {
      Cvc5Term value = cvc5_get_value(solver, vars[i]);
      /* Atenção: cvc5_term_to_string reaproveita um buffer interno, então o
       * resultado só é válido até a próxima chamada. Por isso imprimimos o
       * nome e o valor em dois printf separados. */
      printf("%s: ", cvc5_term_to_string(vars[i]));
      printf("%s\n", cvc5_term_to_string(value));
      /* vars[i] != value */
      Cvc5Term pair[2] = {vars[i], value};
      block[i] = cvc5_mk_term(tm, CVC5_KIND_DISTINCT, 2, pair);
    }
    printf("============\n");
    /* Bloqueia a solução atual, para que a próxima chamada dê outra. */
    cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_OR, 3, block));
    count++;

    cvc5_result_release(res);
    res = cvc5_check_sat(solver);
  }

  /* Liberamos a memória. Todos os termos pertencem ao term manager, então
   * basta apagar o solucionador e o term manager no fim. */
  cvc5_result_release(res);
  cvc5_delete(solver);
  cvc5_term_manager_delete(tm);
  return 0;
}
```

### Determinando a equivalência de duas fórmulas proposicionais

O programa abaixo, ao ser executado, testa a equivalência entre as fórmulas `¬(p ∨ (¬p ∧ q))` e `¬p ∧ ¬q`. Se elas são equivalentes então a bi-implicação entre elas deve ser uma tautologia. O que significa que a *negação* da bi-implicação deve ser uma contradição, ou seja, deve ser *insatisfatível*. Na linguagem do `cvc5`, a bi-implicação é representada com `==` e sua negação com `!=`.

Então, se o resultado for `unsat`, as fórmulas são equivalentes. Caso contrário existe uma maneira de dar valores às suas variáveis que as faz ter valores de verdade diferentes.

**Em Python:**

```python
from cvc5.pythonic import *

if __name__ == '__main__':
    p, q = Bools("p q")
    s = Solver()

    # Testar equivalencia de ¬(p ∨ (¬p ∧ q)) e ¬p ∧ ¬q
    s.add(
        Not(Or(p, (And(Not(p), q)))) != And(Not(p), Not(q)))

    print(s.check())
```

**Em C**, usando o arquivo [`equivalencia.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/equivalencia.c %}):

```c
/* Testa se ¬(p ∨ (¬p ∧ q)) e ¬p ∧ ¬q são equivalentes, usando a API em C do
 * cvc5. Se a negação da bi-implicação entre as duas for insatisfatível
 * (resposta `unsat`), então elas são equivalentes. */

#include <cvc5/c/cvc5.h>
#include <stdio.h>

int main(void)
{
  Cvc5TermManager* tm = cvc5_term_manager_new();
  Cvc5* solver = cvc5_new(tm);

  Cvc5Sort boolean = cvc5_get_boolean_sort(tm);
  Cvc5Term p = cvc5_mk_const(tm, boolean, "p");
  Cvc5Term q = cvc5_mk_const(tm, boolean, "q");

  Cvc5Term np = cvc5_mk_term(tm, CVC5_KIND_NOT, 1, &p);
  Cvc5Term nq = cvc5_mk_term(tm, CVC5_KIND_NOT, 1, &q);

  /* f1 = ¬(p ∨ (¬p ∧ q)) */
  Cvc5Term np_q[2] = {np, q};
  Cvc5Term conj = cvc5_mk_term(tm, CVC5_KIND_AND, 2, np_q);
  Cvc5Term p_conj[2] = {p, conj};
  Cvc5Term disj = cvc5_mk_term(tm, CVC5_KIND_OR, 2, p_conj);
  Cvc5Term f1 = cvc5_mk_term(tm, CVC5_KIND_NOT, 1, &disj);

  /* f2 = ¬p ∧ ¬q */
  Cvc5Term np_nq[2] = {np, nq};
  Cvc5Term f2 = cvc5_mk_term(tm, CVC5_KIND_AND, 2, np_nq);

  /* A negação da bi-implicação f1 ↔ f2 é simplesmente f1 ≠ f2, que em C se
   * escreve com o conectivo CVC5_KIND_DISTINCT (a bi-implicação seria
   * CVC5_KIND_EQUAL). */
  Cvc5Term both[2] = {f1, f2};
  cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_DISTINCT, 2, both));

  Cvc5Result res = cvc5_check_sat(solver);
  printf("%s\n", cvc5_result_to_string(res));

  cvc5_result_release(res);
  cvc5_delete(solver);
  cvc5_term_manager_delete(tm);
  return 0;
}
```

### Resolvendo o problema das n-rainhas

Segundo a codificação dada nas [notas de aula de satisfatibilidade]({{ site.baseurl }}{% link _lessons/01-prop-logic/03-prop-logic-sat.pdf %}), podemos escrever o programa abaixo que dá todas as soluções para o problema das `n` rainhas, para um dado `n`.

**Em Python:**

```python
from cvc5.pythonic import *

# Definindo o número de rainhas
n = 8

if __name__ == '__main__':
    # Criamos n+1 posições para que possamos contar de 1 até n (em vez de 0
    # até n-1, como seria o padrão)
    board = [[None for i in range(n+1)] for i in range(n+1)]
    # Criamos uma variável para cada posição no tabuleiro
    for i in range(1, n+1):
        for j in range(1, n+1):
            board[i][j] = Bool("p{}{}".format(i, j))

    s = Solver()
    # Q1: há pelo menos uma rainha por linha
    for i in range(1, n+1):
        row = []
        for j in range(1, n+1):
            row += [board[i][j]]
        s.add(Or(row))

    # Q2: há no máximo uma rainha por linha
    for i in range(1, n+1):
        for j in range(1, n):
            for k in range(j+1, n+1):
                s.add(Implies(board[i][j], Not(board[i][k])))

    # Q3: há no máximo uma rainha por coluna
    for j in range(1, n+1):
        for i in range(1, n):
            for k in range(i+1, n+1):
                s.add(Implies(board[i][j], Not(board[k][j])))

    # Q4: não há rainhas na mesma diagonal (parte 1)
    for i in range(2, n+1):
        for j in range(1, n):
            for k in range(1, min(i-1, n-j) + 1):
                s.add(Implies(board[i][j], Not(board[i-k][k+j])))

    # Q5: não há rainhas na mesma diagonal (parte 2)
    for i in range(1, n):
        for j in range(1, n):
            for k in range(1, min(n-i, n-j) + 1):
                s.add(Implies(board[i][j], Not(board[i+k][j+k])))

    count = 0
    while (s.check() == sat):
        m = s.model()
        values = []
        count += 1
        print("Solution {}\n----------".format(count))
        for i in range(1, n+1):
            string = ""
            for j in range(1, n+1):
                string += "{}{}".format("Q" if m[board[i][j]] else "_", ", " if j < n else "")
                values += [board[i][j] != m[board[i][j]]]
            print(string)
        print("===============================================")
        # block current solution
        s.add(Or(values))
```

**Em C**, usando o arquivo [`n-rainhas.c`]({{ site.baseurl }}{% link _lessons/01-prop-logic/cvc5-c/n-rainhas.c %}):

```c
/* Resolve o problema das n rainhas com a API em C do cvc5, seguindo a
 * codificação das notas de aula de satisfatibilidade. */

#include <cvc5/c/cvc5.h>
#include <stdio.h>

/* Número de rainhas (e o tamanho do tabuleiro). */
#define N 8

/* Usamos índices de 1 até N (a linha e a coluna 0 ficam sem uso), para que o
 * programa siga de perto a codificação vista em aula. */
static Cvc5Term board[N + 1][N + 1];

/* Atalho para construir ¬t. */
static Cvc5Term mk_not(Cvc5TermManager* tm, Cvc5Term t)
{
  return cvc5_mk_term(tm, CVC5_KIND_NOT, 1, &t);
}

/* Atalho para construir a → b. */
static Cvc5Term mk_implies(Cvc5TermManager* tm, Cvc5Term a, Cvc5Term b)
{
  Cvc5Term args[2] = {a, b};
  return cvc5_mk_term(tm, CVC5_KIND_IMPLIES, 2, args);
}

static int min(int a, int b) { return a < b ? a : b; }

int main(void)
{
  Cvc5TermManager* tm = cvc5_term_manager_new();
  Cvc5* solver = cvc5_new(tm);
  cvc5_set_option(solver, "produce-models", "true");

  Cvc5Sort boolean = cvc5_get_boolean_sort(tm);

  /* Uma variável proposicional para cada posição do tabuleiro. */
  for (int i = 1; i <= N; i++)
  {
    for (int j = 1; j <= N; j++)
    {
      char name[32];
      snprintf(name, sizeof(name), "p%d%d", i, j);
      board[i][j] = cvc5_mk_const(tm, boolean, name);
    }
  }

  /* Q1: há pelo menos uma rainha por linha. */
  for (int i = 1; i <= N; i++)
  {
    Cvc5Term row[N];
    for (int j = 1; j <= N; j++) row[j - 1] = board[i][j];
    cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_OR, N, row));
  }

  /* Q2: há no máximo uma rainha por linha. */
  for (int i = 1; i <= N; i++)
    for (int j = 1; j < N; j++)
      for (int k = j + 1; k <= N; k++)
        cvc5_assert_formula(
            solver, mk_implies(tm, board[i][j], mk_not(tm, board[i][k])));

  /* Q3: há no máximo uma rainha por coluna. */
  for (int j = 1; j <= N; j++)
    for (int i = 1; i < N; i++)
      for (int k = i + 1; k <= N; k++)
        cvc5_assert_formula(
            solver, mk_implies(tm, board[i][j], mk_not(tm, board[k][j])));

  /* Q4: não há rainhas na mesma diagonal (parte 1). */
  for (int i = 2; i <= N; i++)
    for (int j = 1; j < N; j++)
      for (int k = 1; k <= min(i - 1, N - j); k++)
        cvc5_assert_formula(
            solver, mk_implies(tm, board[i][j], mk_not(tm, board[i - k][k + j])));

  /* Q5: não há rainhas na mesma diagonal (parte 2). */
  for (int i = 1; i < N; i++)
    for (int j = 1; j < N; j++)
      for (int k = 1; k <= min(N - i, N - j); k++)
        cvc5_assert_formula(
            solver, mk_implies(tm, board[i][j], mk_not(tm, board[i + k][j + k])));

  int count = 0;
  Cvc5Result res = cvc5_check_sat(solver);
  while (cvc5_result_is_sat(res))
  {
    Cvc5Term block[N * N];
    size_t nblock = 0;
    count++;
    printf("Solution %d\n----------\n", count);
    for (int i = 1; i <= N; i++)
    {
      for (int j = 1; j <= N; j++)
      {
        Cvc5Term value = cvc5_get_value(solver, board[i][j]);
        printf("%s%s", cvc5_term_get_boolean_value(value) ? "Q" : "_",
               j < N ? ", " : "\n");
        /* board[i][j] != value */
        Cvc5Term pair[2] = {board[i][j], value};
        block[nblock++] = cvc5_mk_term(tm, CVC5_KIND_DISTINCT, 2, pair);
      }
    }
    printf("===============================================\n");
    /* Bloqueia a solução atual. */
    cvc5_assert_formula(solver, cvc5_mk_term(tm, CVC5_KIND_OR, nblock, block));

    cvc5_result_release(res);
    res = cvc5_check_sat(solver);
  }
  printf("Total: %d solução(ões)\n", count);

  cvc5_result_release(res);
  cvc5_delete(solver);
  cvc5_term_manager_delete(tm);
  return 0;
}
```

## Tutorial de SMT

O cvc5 não é apenas um solucionador SAT: ele é um solucionador **SMT**, de
*satisfatibilidade módulo teorias*. A ideia é a seguinte. Em SAT, as variáveis
de uma fórmula são proposições que só podem ser verdadeiras ou falsas. Em SMT,
as fórmulas continuam sendo fórmulas proposicionais — com os mesmos conectivos
`∧`, `∨`, `¬`, `→` que estudamos aqui —, mas no lugar das variáveis
proposicionais podemos ter afirmações sobre *outros* domínios: números
inteiros, vetores de bits, arranjos, cadeias de caracteres, e assim por diante.
Por exemplo, `(x + y > 3) ∧ ¬(x < 1)` é uma fórmula SMT sobre aritmética. Tudo
o que aprendemos sobre a estrutura lógica das fórmulas continua valendo; o que
muda é o que as folhas da fórmula podem dizer.

Para quem quiser ir além do que vemos em aula, o time do cvc5 mantém um
tutorial introdutório muito bom:

- [**Beginner's Tutorial on SMT**](https://cvc5.github.io/tutorials/beginners/index.html)

Ele é escrito para quem está começando e não pressupõe experiência prévia com
SAT ou SMT. Cobre uma introdução geral, os fundamentos formais (sintaxe,
semântica, teorias), um capítulo para cada uma das principais teorias
(aritmética, arranjos, vetores de bits, tipos de dados, ponto flutuante, ...) e
como interpretar as respostas de um solucionador (`sat`, `unsat` e `unknown`).
Todos os capítulos têm exercícios com solução, que podem ser resolvidos tanto
em SMT-LIB quanto pelas APIs.

Um roteiro sugerido para quem quer aprender a usar essas ferramentas:

1. **Comece pelos exemplos desta página.** Reescreva-os, mude as fórmulas, veja
   o que acontece. Entender bem o laço "verifique, extraia o modelo, bloqueie a
   solução, repita" já dá conta de uma boa quantidade de problemas.
2. **Aprenda o SMT-LIB.** É a linguagem-padrão de entrada dos solucionadores
   SMT, e é bem mais direta que escrever um programa. Você pode escrever um
   arquivo `problema.smt2` e rodar `cvc5 problema.smt2` na linha de comando. A
   [documentação do cvc5](https://cvc5.github.io/docs/latest/) descreve a
   linguagem e as opções disponíveis, e os capítulos do tutorial acima usam
   SMT-LIB o tempo todo.
3. **Faça os exercícios do tutorial**, um capítulo por teoria. A parte de
   aritmética linear (`LIA`/`LRA`) é a mais útil para começar.
4. **Modele um problema seu.** A habilidade que realmente importa não é usar a
   API, e sim *codificar* um problema como uma fórmula lógica — foi exatamente
   isso que fizemos com as `n` rainhas. Sudoku, coloração de grafos, alocação
   de horários e verificação de circuitos são bons exercícios seguintes.

Voltaremos a falar de SMT em aulas futuras.
