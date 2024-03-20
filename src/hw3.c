#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
void free_game_state(GameState *game_state) {
    if (game_state) {
        if (game_state->board) {
            for (int i = 0; i < game_state->rows; i++) {
                if (game_state->board[i]) {
                    for (int j = 0; j < game_state->cols; j++) {
                        free(game_state->board[i][j].top);
                    }
                    free(game_state->board[i]);
                }
            }
            free(game_state->board);
        }
        free(game_state);
    }
}

// Function to initialize the game state
GameState* initialize_game_state(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        exit(1);
    }

    int num_rows = 0;
    int num_cols = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            num_rows++;
        } else if (num_rows == 0) {
            num_cols++;
        }
    }
    rewind(file);

    GameState *game_state = (GameState*)malloc(sizeof(GameState));
    if (game_state == NULL) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed when initializing the board.\n");
        exit(1);
    }

    game_state->rows = num_rows;
    game_state->cols = num_cols;
    game_state->board = (Tile**)malloc(num_rows * sizeof(Tile*));
    if (game_state->board == NULL) {
        fclose(file);
        free_game_state(game_state);
        fprintf(stderr, "Memory allocation failed when initializing the game_state struct to board.\n");
        exit(1);
    }

    for (int i = 0; i < num_rows; i++) {
        game_state->board[i] = (Tile*)malloc(num_cols * sizeof(Tile));
        if (game_state->board[i] == NULL) {
            fclose(file);
            free_game_state(game_state);
            fprintf(stderr, "Memory allocation failed when inilizing a tile.\n");
            exit(1);
        }

        for (int j = 0; j < num_cols; j++) {
            ch = fgetc(file);
            if (ch == '.') {
                game_state->board[i][j].top = NULL;
                game_state->board[i][j].height = 0;
            } else {
                game_state->board[i][j].top = malloc(sizeof(char));
                *(game_state->board[i][j].top) = ch;
                game_state->board[i][j].height = 1;
            }
        }
        fgetc(file); 
    }

    fclose(file);
    return game_state;
}

int is_valid_position(GameState *game, int row, int col) {
    return row >= 0 && col >= 0 && row < game->rows && col < game->cols;
}

int can_place_tile(GameState *game, int row, int col, char tile) {
    if (!is_valid_position(game, row, col)) {
        return 0; 
    }
    if (game->board[row][col].height >= 5) {
        return 0; 
    }
    if (tile == ' ' && game->board[row][col].top == NULL) {
        return 0; 
    }
    if (game->board[row][col].top != NULL && *game->board[row][col].top == tile) {
        return 0; 
    }
    return 1; 
}

void board_resize(GameState *game, int new_rows, int new_cols) {
    Tile **new_board = (Tile **)malloc(new_rows * sizeof(Tile *));
    if (new_board == NULL) {
      printf("Memory allocation failed to create new board rows\n");
        exit(1);
    }

    for (int i = 0; i < new_rows; i++) {
        new_board[i] = (Tile *)malloc(new_cols * sizeof(Tile));
        if (new_board[i] == NULL) {
            for (int k = 0; k < i; k++) {
                free(new_board[k]);
            }
            free(new_board);
            printf( "Memory allocation failed to create new board cols\n");
            exit(1);
        }

        for (int j = 0; j < new_cols; j++) {
           new_board[i][j].top = malloc(sizeof(char)); 
            if (new_board[i][j].top != NULL) {
                *(new_board[i][j].top) = '.';
            } else {
                printf("Failed to allocate memory for new tile");
                exit(1);
            }
            new_board[i][j].height = 0;

            // Copy data from old board
            if (i < game->rows && j < game->cols && game->board[i][j].top != NULL) {
                new_board[i][j].top = malloc(sizeof(char)); 
                if (new_board[i][j].top == NULL) {
                    printf("Failed to allocate memory for new tile at (%d,%d)\n", i, j);
                    exit(1);
                }
                *(new_board[i][j].top) = *(game->board[i][j].top); // Copy the character
                new_board[i][j].height = game->board[i][j].height; // Copy the height
            }
        }
    }

  
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            free(game->board[i][j].top); 
        }
        free(game->board[i]);
    }
    free(game->board);

    game->board = new_board;
    game->rows = new_rows;
    game->cols = new_cols;
}


GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    *num_tiles_placed=0;
    int endRow = row + (direction == 'V' ? strlen(tiles) - 1 : 0);
    int endCol = col + (direction == 'H' ? strlen(tiles) - 1 : 0);
//    printf("%d", endRow);
//    printf("%d\n", endCol);
//    printf("%d", game->rows);
//    printf("%d", game->cols);
   
    if ((endRow >= game->rows )) {
        board_resize(game, endRow+1, game->cols);
    } else if(endCol >= game->cols){
         board_resize(game, game->rows, endCol+1);
    }else if(endRow >= game->rows  &&endCol >= game->cols){
        board_resize(game, endRow+1, endCol+1);
    }

    int len = strlen(tiles);
    for (int i = 0; i < len; ++i) {
        int currentRow = row + (direction == 'V' ? i : 0);
        int currentCol = col + (direction == 'H' ? i : 0);
        // printf("%d",currentRow);
        // printf("%d",currentColhe);

        if (!can_place_tile(game, currentRow, currentCol, tiles[i])) {
            return game; 
        }
       
        if (tiles[i] != ' ') { 
            if (game->board[currentRow][currentCol].top == NULL) {
                game->board[currentRow][currentCol].top = malloc(sizeof(char));
                if (game->board[currentRow][currentCol].top == NULL) {
                    free_game_state(game);
                    fprintf(stderr, "Memory allocation failed when placing a tile.\n");
                    exit(1);
                }
                *game->board[currentRow][currentCol].top = tiles[i];
                game->board[currentRow][currentCol].height = 1;
            } else {
                *game->board[currentRow][currentCol].top = tiles[i];
                //  printf("%d",*game->board[currentRow][currentCol].top);
                game->board[currentRow][currentCol].height++;
            }
            (*num_tiles_placed)++;
        }
    }

    return game;
}


GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return NULL;
}



void save_game_state(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing.\n");
        exit(EXIT_FAILURE);
    }

   
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            if (game->board[i][j].top != NULL) {
                fprintf(file, "%c", *(game->board[i][j].top));
            } else {
                fprintf(file, ".");
            }
        }
        fprintf(file, "\n");
    }
    
   
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            fprintf(file, "%d", game->board[i][j].height); 
        }
        fprintf(file, "\n"); 
    }

    fclose(file);
}