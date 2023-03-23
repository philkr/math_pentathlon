import numpy as np
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument('-s', '--show-solution', action='store_true')
args = parser.parse_args()

solution = np.fromfile('calla.bin', dtype=[('hash', '<u8'), ('score', '<i1')])

def printc(state, ds = None):
    print(' ', ' '.join([str(s) for s in state[5:][::-1]]))
    if ds is not None:
        tot = 32 - np.sum(state)
        print((tot - ds) // 2, ' '*9, (tot + ds) // 2)
    print(' ', ' '.join([str(s) for s in state[:5]]))

def hash(state):
    return sum([(s << (6*i)) for i, s in enumerate(state)])

def move(state, p, i):
    s0 = [x for x in state]
    n = s0[i + 5*p]
    s0[i + 5*p] = 0
    calla = 0

    while n>0:
        for j in range(i+1, 5):
            s0[j + 5*p] += 1
            if n == 1 and s0[j + 5*p]==1:
                calla += s0[4-j + 5*(1-p)]
                s0[4-j + 5*(1-p)] = 0
            if n == 1:
                return s0, calla, False
            n -= 1
        calla += 1
        if n==1:
            return s0, calla, True
        n -= 1
        for j in range(5):
            s0[j + 5*(1-p)] += 1
            if n == 1:
                return s0, calla, False
            n -= 1
        i = -1

def move_score(state, p, i):
    if not state[i + 5*p]:
        return -100
    s1, score, again = move(state, p, i)
    if not again:
        p = 1 - p
    if p:
        s1_h = hash(s1[5:] + s1[:5])
    else:
        s1_h = hash(s1)
    assert (s1_h == solution['hash']).any()
    return score + (2*again-1) * int(solution['score'][s1_h == solution['hash']])


def move_scores(state, p):
    return [move_score(state, p, i) for i in range(5)]


def play(p1, p2, show_score=False):
    players = [p1, p2]
    state = [3, 3, 4, 3, 3, 3, 3, 4, 3, 3]
    score = 0
    p = 0
    for _ in range(100):
        print('-'*10)
        if show_score:
            print(move_scores(state, p))
        printc(state, ds=score)
        if np.sum(state[p*5:p*5+5]) == 0:
            break
        m = players[p](state, p)
        while not state[p*5+m]:
            m = players[p](state, p)
        print(f'M: {p} {m}')
        state, d_score, again = move(state, p, m)
        score += (1-2*p) * d_score
        if not again:
            p = 1 - p

def io_player(*args):
    return int(input('Move:'))

def ai_player(state, p):
    return np.argmax(move_scores(state, p))

play(io_player, ai_player, show_score=args.show_solution)
