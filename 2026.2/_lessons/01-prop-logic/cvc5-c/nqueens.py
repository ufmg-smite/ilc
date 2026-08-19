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
