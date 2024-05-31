# Project: Upwords Game Implementation

## Overview
This project involves implementing a simplified version of the Upwords game. The project consists of several key functions to initialize, place tiles, undo moves, save, and free the game state.

## Rules to Implement

1. **Board Coordinates**: The upper-left corner of the game board is located at row #0, column #0.
2. **Turn Mechanics**: A turn involves placing one or more tiles that connect to or modify an existing word. All newly created or modified words must be legal.
3. **Tile Placement**: All letters must be placed in the same row or column.
4. **Letter Case**: All letters placed on the board are uppercase.
5. **First Word**: The first word can be placed anywhere on the board and must contain at least two letters.
6. **Board Expansion**: The board can grow horizontally or vertically to accommodate new words if they would be legal on a bigger board and do not create illegal words. The minimum number of rows or columns must be added.
7. **Stacking Tiles**: Tiles can be stacked on existing tiles to form new legal words. A stack may be at most 5 tiles high. At least one letter of the original board must remain uncovered and used in the new word.
8. **Legal Words**: A file named `words.txt` contains all legal words. Any word in this file is considered legal. Adding an "S" is allowed if it creates a new word in the `words.txt` file.


## Key Functions

### 1. `GameState* initialize_game_state(const char *filename)`
This function initializes a new `GameState` struct from a file representing the state of a valid game. All stacks of tiles are assumed to be 1 tile high. 
### 2. `GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed)`
This function attempts to place the given tiles on the board. The direction indicates whether the placement is vertical ('V') or horizontal ('H'). It returns a pointer to the updated `GameState` struct or the original game state if placement is invalid.
### 3. `GameState* undo_place_tiles(GameState *game)`
This function undoes the effects of the most recent successful call to `place_tiles`. If there is no previous state to revert to, it simply returns the current game state.
### 4. `void free_game_state(GameState *game)`
This function deallocates all memory associated with the game state.
### 5. `void save_game_state(GameState *game, const char *filename)`
This function saves the current game state to a file. The saved file includes the top tile of each stack and the height of each stack.


## Getting Started
To get started, follow these steps:
1. Clone the repository to your local machine.
2. Initialize the game state from a file using `initialize_game_state`.
3. Use `place_tiles` to play the game and `undo_place_tiles` to revert moves if necessary.
4. Save the game state using `save_game_state`.
5. Free the game state memory using `free_game_state`.
