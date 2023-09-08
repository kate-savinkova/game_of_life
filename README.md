# Game of life

This project - visualisation of a ***game of life***, popular math game

***Main rules:***

Each square (or "cell") on the grid can be either alive or dead, and they evolve according to the following rules:

- Any live cell with < 2 live neighbours dies (referred to as underpopulation).
- Any live cell with > 3 live neighbours dies (referred to as overpopulation).
- Any live cell with 2 or 3 live neighbours lives, unchanged, to the next generation.
- Any dead cell with exactly 3 live neighbours comes to life.

Project includes files with templates of starting fields, which can be passed by the stdio (./a.out < 5.txt)

There're also special settings of the speed and condition of finishing a game:

`a`: to make game be faster

`z`: to make game be slower

`q`: to finish the game