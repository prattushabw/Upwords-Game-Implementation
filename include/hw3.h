#include <stdlib.h>

typedef struct Tile{
    char* top;
    int height;
} Tile;

typedef struct GameState
{
    Tile **board;
    int rows;
    int cols;
} GameState;

typedef struct GameHistory{
    GameState **states;
    int top;
    int size;
    int capacity;
}GameHistory;

GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
