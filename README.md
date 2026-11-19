# Anti-Maze

A console-based C maze game where you navigate a maze to collect subatomic particles and produce antimatter while racing against a timer and avoiding black holes.

## Features

- **Manual Mode**: Navigate with arrow keys (`Esc` to quit) under a map-specific time limit.
- **Automatic Mode**: Recursive pathfinding solver that checks whether a map is solvable and prints the solution coordinates.
- **Account & Scoreboard System**: User registration/login (`login.dat`) and a top-5 leaderboard (`scores.txt`).
- **Map Importing**: Import custom text maps (see `examplemap1.dat`) into binary storage (`maps.bin`).

## Map Symbols

| Symbol    | Meaning                           |
| :-------- | :-------------------------------- |
| `X`       | Player                            |
| `0`       | Empty path                        |
| `1`       | Wall                              |
| `C`       | Exit                              |
| `K`       | Micro black hole (instant loss)   |
| `p` / `P` | Proton (`P+`) / Antiproton (`P-`) |
| `e` / `E` | Electron (`e-`) / Positron (`e+`) |

Collect antiprotons (`P`) and positrons (`E`) to form antimatter; matching particle-antiparticle pairs annihilate each other.

## Build & Run

> **Note:** Uses `<conio.h>` (`_getch()`) for arrow-key input, targeting Windows.

```bash
gcc main.c -o anti-maze.exe
./anti-maze.exe
```
