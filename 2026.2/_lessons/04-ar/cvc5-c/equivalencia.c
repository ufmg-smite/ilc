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
