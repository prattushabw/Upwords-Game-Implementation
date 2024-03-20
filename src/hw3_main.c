#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

void print_board(GameState *game) {
    if (game == NULL || game->board == NULL) {
        printf("The game is not initialized.\n");
        return;
    }

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            if (game->board[i][j].top != NULL) {
                printf("%c ", *game->board[i][j].top);
            } else {
                printf(". ");
            }
        }
        printf("\n"); 

    }

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
                printf("%i ", game->board[i][j].height);
        }
        
        printf("\n"); 
    }
}


int main(void) {
    int num_tiles_placed=0;
   GameState *game = initialize_game_state("./tests/boards/board01.txt"); 
    if (!game) {
        fprintf(stderr, "Failed to initialize game.\n");
        return EXIT_FAILURE;
    }
    print_board(game);
    printf("\n\n");
 
    game = place_tiles(game, 3, 2, 'H', "C", &num_tiles_placed);
    print_board(game);

    save_game_state(game, "./tests/actual_outputs/output.txt");
    printf("TilesPlaced: %d\n", num_tiles_placed);
    free_game_state(game);

    return EXIT_SUCCESS;
}
  