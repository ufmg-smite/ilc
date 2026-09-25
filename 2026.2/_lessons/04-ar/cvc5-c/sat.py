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
