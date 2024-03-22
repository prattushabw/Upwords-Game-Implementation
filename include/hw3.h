#include <stdlib.h>

typedef struct Tile{
    char* top;
    int height;
} Tile;
typedef struct GameState GameState;

typedef struct GameHistory {
    GameState **snapshots; // An array of pointers to game state snapshots
    int count; // Number of snapshots currently stored
    int capacity; // Current capacity of the snapshots array
} GameHistory;

typedef struct GameState
{
    Tile **board;
    int rows;
    int cols;
    GameHistory *history;
} GameState;



GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
