from cvc5.pythonic import *

if __name__ == '__main__':
    p, q = Bools("p q")
    s = Solver()

    # Testar equivalencia de ¬(p ∨ (¬p ∧ q)) e ¬p ∧ ¬q
    s.add(
        Not(Or(p, (And(Not(p), q)))) != And(Not(p), q))

    print(s.check())
