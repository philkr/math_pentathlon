#include <unordered_map>
#include <tuple>
#include <array>
#include <cstdint>


typedef uint64_t CState;
#define I(p, j) static_cast<CState>(5*(p)+(j))
// Index into the data s[p][j]
#define G(s, p, j) ((s >> (6*I(p, j))) & 0x3f)
// s += A(p, j)  is equivalent to s[p][j] += 1
#define A(p, j) (1ull<<(6*I(p, j)))
// Zero a certain state s[p][j] = 0
#define Z(s, p, j) (s -= (A(p, j) * G(s, p, j)))
#define FLIP(s) (((s >> 30) & 0x3fffffff) | ((s & 0x3fffffff) << 30))

std::tuple<CState, int, bool> move(CState s, int p, int i) {
    int calla = 0;
    uint32_t n = G(s, p, i);
    Z(s, p, i);
    // s[p][i] = 0;
    while (n>0) {
        for (int j=i+1; j<5; j++, n--) {
            if (G(s, p, j) == 0 && n==1) {
                // Capture
                calla += G(s, !p, 4-j);
                Z(s, !p, 4-j);
            }
            s += A(p, j);
            if (n==1)
                return std::make_tuple(s, calla, false);
        }
        calla += 1;
        if (n==1)
            return std::make_tuple(s, calla, true);
        n-=1;
        for (int j=0; j<5; j++, n--) {
            s += A(!p, j);
            if (n==1)
                return std::make_tuple(s, calla, false);
        }
        i=-1;
    }
    assert(false);
}

static uint64_t n_eval[2] = {0, 0};
static std::unordered_map<CState, int> memory;
uint32_t solve(CState s, int p) {
    if (p == 0 && memory.count(s))
        return memory[s];
    if (p == 1 && memory.count(FLIP(s)))
        return memory[FLIP(s)];

    if ((n_eval[0] + n_eval[1]) % 10000000 == 0) {
        printf("Stats %lld %lld %lu\n", n_eval[0], n_eval[1], memory.size());
    }
    n_eval[p]++;

    int best_score = -100;
    for(int i=0; i<5; i++) {
        if (G(s, p, i)) {
            CState ss = s;
            int score = 0;
            bool again = 0;

            // printf("  M %d [%d %d %d %d %d] [%d %d %d %d %d]\n", i, s.s[0][0], s.s[0][1], s.s[0][2], s.s[0][3], s.s[0][4], s.s[1][0], s.s[1][1], s.s[1][2], s.s[1][3], s.s[1][4]);
            std::tie(ss, score, again) = move(s, p, i);
            // printf("  done [%d %d %d %d %d] [%d %d %d %d %d]\n", s.s[0][0], s.s[0][1], s.s[0][2], s.s[0][3], s.s[0][4], s.s[1][0], s.s[1][1], s.s[1][2], s.s[1][3], s.s[1][4]);
            if (again)
                score += solve(ss, p);
            else
                score -= solve(ss, !p);

            if (best_score < score)
                best_score = score;
        }
    }
    if (best_score == -100)
        best_score = 0;
    if (p==0)
        memory[s] = best_score;
    if (p==1)
        memory[FLIP(s)] = best_score;
    return best_score;
}

CState init_state() {
    CState s = 0;
    for (int i=0; i<10; i++)
        s += A(0, i) * (3 + ((i % 5) == 2));
    return s;
}

int main() {
    uint32_t s = solve(init_state(), 0);
    printf("%d\n", s);
    FILE * fp = fopen("calla.bin", "wb");
    for (const auto & m: memory) {
        fwrite(&m.first, sizeof(CState), 1, fp);
        int8_t v = m.second;
        fwrite(&v, sizeof(v), 1, fp);
    }
    fclose(fp);
    return 0;
}
