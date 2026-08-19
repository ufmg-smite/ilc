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
