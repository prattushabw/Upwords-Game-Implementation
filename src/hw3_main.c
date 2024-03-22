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
    GameState *game = initialize_game_state("./tests/boards/board06.txt");
    if (!game) {
        fprintf(stderr, "Failed to initialize game.\n");
        return EXIT_FAILURE;
    }
    print_board(game);
    printf("\n\n");

    game = undo_place_tiles(game);
    game = place_tiles(game, 8, 5, 'V', "SOME", &num_tiles_placed);
   
    game = place_tiles(game, 9, 17, 'V', "ABBAGE", &num_tiles_placed);
   
    game = place_tiles(game, 0, 0, 'H', "CAT", &num_tiles_placed);
    
    game = place_tiles(game, 8, 5, 'V', "SOME", &num_tiles_placed);
  
    game = place_tiles(game, 10, 5, 'H', " ANGANOCOLUM ITE", &num_tiles_placed);
 
    game = place_tiles(game, 10, 5, 'H', "HOME", &num_tiles_placed);
    
    game = undo_place_tiles(game);
    game = undo_place_tiles(game);
    game = undo_place_tiles(game);
    
   
    print_board(game);

    save_game_state(game, "./tests/actual_outputs/output.txt");
    printf("TilesPlaced: %d\n", num_tiles_placed);
    free_game_state(game);

    return EXIT_SUCCESS;
}
  