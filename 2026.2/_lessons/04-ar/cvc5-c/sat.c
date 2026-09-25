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
