# Solutions to math pentathlon games

For now, Calla, but Hexagone may have a sufficiently small state space too.

## Calla

Step 1: Run the Calla solver (this will create a 1.4GB file! Make sure you have the disk space.)
```bash
g++ calla/solver.cpp -o solver -O3 -std=c++14
./solver
```
The solver will take a while (~5min on a MBP).

Step 2: Play the game
```bash
python3 calla/calla.py
```

You select your bin 0-4 (0: leftmost, 1: rightmost).

If you want to see a score for each potential move run
```bash
python3 calla/calla.py -s
```
The score shows the difference in score each move would cause (if both players played optimally after).
For example, if you were to score 4 more points and the AI 6 more you would see -2.