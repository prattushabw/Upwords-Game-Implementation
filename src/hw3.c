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

GameHistory* initialize_game_history() {
    GameHistory *history = malloc(sizeof(GameHistory));
    history->snapshots = malloc(10 * sizeof(GameState*)); // Initial capacity
    history->count = 0;
    history->capacity = 10;
    return history;
}

GameState* initialize_game_state(const char *filename) {
    FILE *file = fopen(filename, "r");


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
        return NULL;
    }

    game_state->rows = num_rows;
    game_state->cols = num_cols;
    game_state->board = (Tile**)malloc(num_rows * sizeof(Tile*));
    if (game_state->board == NULL) {
        fclose(file);
        free_game_state(game_state);
        fprintf(stderr, "Memory allocation failed when initializing the game_state struct to board.\n");
        return NULL;
    }

    for (int i = 0; i < num_rows; i++) {
        game_state->board[i] = (Tile*)malloc(num_cols * sizeof(Tile));
        if (game_state->board[i] == NULL) {
            fclose(file);
            free_game_state(game_state);
            fprintf(stderr, "Memory allocation failed when inilizing a tile.\n");
            return NULL;
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
    game_state->history = initialize_game_history();
   
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
       return;
    }

    for (int i = 0; i < new_rows; i++) {
        new_board[i] = (Tile *)malloc(new_cols * sizeof(Tile));
        if (new_board[i] == NULL) {
            for (int k = 0; k < i; k++) {
                free(new_board[k]);
            }
            free(new_board);
            printf( "Memory allocation failed to create new board cols\n");
            return;
        }

        for (int j = 0; j < new_cols; j++) {
           new_board[i][j].top = malloc(sizeof(char)); 
            if (new_board[i][j].top != NULL) {
                *(new_board[i][j].top) = '.';
            } else {
                printf("Failed to allocate memory for new tile");
               return;
            }
            new_board[i][j].height = 0;

            // Copy data from old board
            if (i < game->rows && j < game->cols && game->board[i][j].top != NULL) {
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

GameState* deep_copy_game_state(const GameState *src) {
    GameState *copy = malloc(sizeof(GameState));
    if (!copy) {
        printf("Failed to allocate memory for game state copy.\n");
        return NULL;
    }
    
    copy->rows = src->rows;
    copy->cols = src->cols;
    copy->history = NULL; 

  
    copy->board = (Tile**)malloc(copy->rows * sizeof(Tile*));
    if (!copy->board) {
        printf("Failed to allocate memory for board copy.\n");
        free(copy); 
        return NULL;
    }

    for (int i = 0; i < copy->rows; i++) {
        copy->board[i] = (Tile*)malloc(copy->cols * sizeof(Tile));
        if (!copy->board[i]) {
            printf("Failed to allocate memory for row %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(copy->board[j]);
            }
            free(copy->board);
            free(copy);
            return NULL;
        }

        for (int j = 0; j < copy->cols; j++) {
            if (src->board[i][j].top) {
                copy->board[i][j].top = malloc(sizeof(char));
                if (!copy->board[i][j].top) {
                    printf("Failed to allocate memory for tile at (%d,%d).\n", i, j);
                    for (int k = 0; k <= i; k++) {
                        for (int l = (k == i) ? 0 : j; l < copy->cols; l++) {
                            free(copy->board[k][l].top);
                        }
                        free(copy->board[k]);
                    }
                    free(copy->board);
                    free(copy);
                    return NULL;
                }
                *(copy->board[i][j].top) = *(src->board[i][j].top);
            } else {
                copy->board[i][j].top = NULL;
            }
            copy->board[i][j].height = src->board[i][j].height;
        }
    }

    return copy;
}


void record_game_state(GameHistory *history, GameState *state) {
    if (history->count >= history->capacity) {
        history->capacity *= 2;
        history->snapshots = realloc(history->snapshots, history->capacity * sizeof(GameState*));
    }
    history->snapshots[history->count++] = deep_copy_game_state(state);
}

void free_game_history(GameHistory *history) {
    if (!history) return; 
    for (int i = 0; i < history->count; i++) {
        free_game_state(history->snapshots[i]); 
    }

    free(history->snapshots); 
    free(history); 
}

char* captureFormedWord(const GameState *game, int row, int col, char direction, const char *tiles) {
    int len = strlen(tiles);
    int startRow = row, startCol = col;
    int endRow = row, endCol = col;

   
    if (direction == 'H') {
        // Check left
        while (startCol > 0 && game->board[startRow][startCol - 1].top != NULL) {
            startCol--;
        }
        // Check right
        endCol = col + len - 1;
        while (endCol < game->cols - 1 && game->board[endRow][endCol + 1].top != NULL) {
            endCol++;
        }
    } else if (direction == 'V') {
        // Check up
        while (startRow > 0 && game->board[startRow - 1][startCol].top != NULL) {
            startRow--;
        }
        // Check down
        endRow = row + len - 1;
        while (endRow < game->rows - 1 && game->board[endRow + 1][endCol].top != NULL) {
            endRow++;
        }
    }

   
    int wordLen = (direction == 'H') ? (endCol - startCol + 1) : (endRow - startRow + 1);
    char *word = malloc(wordLen + 1); 

   
    for (int i = 0; i < wordLen; i++) {
        if (direction == 'H') {
            if (game->board[row][startCol + i].top != NULL) {
                word[i] = *game->board[row][startCol + i].top;
            } else {
                word[i] = '.';
            }
        } else {
            if (game->board[startRow + i][col].top != NULL) {
                word[i] = *game->board[startRow + i][col].top;
            } else {
                word[i] = '.';
            }
        }
    }
    word[wordLen] = '\0'; 

    char *temp = word;  
    while (*temp) {
        *temp = tolower((unsigned char) *temp);
        temp++;
    }

    return word;
}

bool isWordValid(const char* word) {

    // FILE* file = fopen("words.txt", "r");
    FILE *file = fopen("./tests/words.txt", "r");
    
    char fileWord[100]; 

    if (file == NULL) {
        printf("Could not open words.txt.\n");
        return false;
    }

    while (fscanf(file, "%99s", fileWord) == 1) { 
        if (strcmp(fileWord, word) == 0) { 
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false; 
}

GameState* undo_place_tiles(GameState *game) {
    if (!game || !game->history || game->history->count == 0) {
        printf("No moves to undo.\n");
        return game;
    }

    int lastIndex = --game->history->count;
    GameState* newState = game->history->snapshots[lastIndex];

    // free the current game state's contents
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
             if (game->board[i][j].top) {
                free(game->board[i][j].top);
            } 
        }
        free(game->board[i]);
    }
    free(game->board);
   
    game->board = newState->board;
    game->rows = newState->rows;
    game->cols = newState->cols;
   
    game->history->snapshots[lastIndex] = NULL;
   

    return game;
}



GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    if (!game || !game->board) {
        printf("Game or game board is NULL.\n");
        return NULL;
    }


    GameState *currentStateCopy = deep_copy_game_state(game);

    if (!currentStateCopy) {
        printf("Failed to record the current game state.\n");
        return NULL; 
    }
    record_game_state(game->history, currentStateCopy);
    free_game_state(currentStateCopy);

    *num_tiles_placed = 0;

    int endRow = row;
    if (direction == 'V') {
        endRow += strlen(tiles) - 1;
    } else {
        endRow += 0;
    }

    int endCol = col;
    if (direction == 'H') {
        endCol += strlen(tiles) - 1;
    } else {
        endCol += 0;
    } 
   
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
      

        if (!can_place_tile(game, currentRow, currentCol, tiles[i])) {
            return game; 
        }
       
        if (tiles[i] != ' ') { 
            if (game->board[currentRow][currentCol].top == NULL) {
                game->board[currentRow][currentCol].top = malloc(sizeof(char));
                if (game->board[currentRow][currentCol].top == NULL) {
                    free_game_state(game);
                    printf("Memory allocation failed when placing a tile.\n");
                    return game;
                }
                *game->board[currentRow][currentCol].top = tiles[i];
                game->board[currentRow][currentCol].height = 1;
            } else {
                *game->board[currentRow][currentCol].top = tiles[i];
                game->board[currentRow][currentCol].height++;
            }
            (*num_tiles_placed)++;
        }
    }

    char* formedWord = captureFormedWord(game, row, col, direction, tiles);
    printf("%s\n", formedWord);
    if (!isWordValid(formedWord)) {
        printf("Placed word is not valid according to words.txt.\n");
        return undo_place_tiles(game);
    }
    free(formedWord);

    return game;
}

void save_game_state(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf( "Error opening file for writing.\n");
        return;
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

    free_game_history(game->history);

    fclose(file);
}

