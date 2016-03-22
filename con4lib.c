#include "con4lib.h"

// Initializes the game so that it's ready to begin.
void init_board(struct connect4 *game) {

    int i, j;

    // Make each game square empty.
    for (i = 0; i < NUM_ROWS; i++)
        for (j = 0; j < NUM_COLS; j++)
            game->board[i][j] = EMPTY;

    // Player one gets to go first!
    game->whoseTurn = PLAYERONE;
}

// Returns 1 iff curmove is NOT a valid column to place a piece in game.
// Returns 0 otherwise.
int not_valid(const struct connect4 *game, int curmove) {

    // These columns don't exist on the board.
    if (curmove < 0 || curmove >= NUM_COLS)
        return 1;

        // Here we check to see if the column is full.
    else if (game->board[NUM_ROWS - 1][curmove] != EMPTY)
        return 1;

    // Otherwise, we're okay.
    return 0;
}

// Pre-condition: move MUST BE a valid column to move in game.
// Returns the row number of where a piece would be placed at this point in
// the game in the column move.
int get_row(const struct connect4 *game, int move) {

    int i = NUM_ROWS;

    // Move through each row, until you hit a piece.
    while (i > 0 && game->board[i - 1][move] == EMPTY) i--;

    // Return this row number.
    return i;
}


// Prints out a representation of the current board
void print_board(const struct connect4 *game) {

    int i, j;

    printf("Here is the game board:\n\n");

    // Print the column numbers above the board.
    for (i = 0; i < NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");

    // Go through each row of the board.
    for (i = NUM_ROWS - 1; i >= 0; i--) {

        for (j = 0; j < NUM_COLS; j++)
            printf("%c ", game->board[i][j]);
        printf("\n");
    }

    // Print the column numbers below the board.
    for (i = 0; i < NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");
}

// Returns the current status of game.
int check_status(const struct connect4 *game) {

    int i, j, k;

    // We go through each row, to look for a horizontal win.
    for (j = 0; j < NUM_ROWS; j++) {

        // We iterate through the possible column starting positions of four
        // consecutive winning pieces.
        for (i = 0; i < NUM_COLS - 3; i++) {

            if ((game->board[j][i] == PLAYERONE) && (game->board[j][i + 1] == PLAYERONE) &&
                (game->board[j][i + 2] == PLAYERONE) && (game->board[j][i + 3] == PLAYERONE))

                return X_WINS;

            else if ((game->board[j][i] == PLAYERTWO) && (game->board[j][i + 1] == PLAYERTWO) &&
                     (game->board[j][i + 2] == PLAYERTWO) && (game->board[j][i + 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We go through each column, to look for a vertical win.
    for (j = 0; j < NUM_COLS; j++) {

        // We iterate through possible row starting positions of four 
        // consecutive winning pieces.
        for (i = 0; i < NUM_ROWS - 3; i++) {

            if ((game->board[i][j] == PLAYERONE) && (game->board[i + 1][j] == PLAYERONE) &&
                (game->board[i + 2][j] == PLAYERONE) && (game->board[i + 3][j] == PLAYERONE))

                return X_WINS;

            else if ((game->board[i][j] == PLAYERTWO) && (game->board[i + 1][j] == PLAYERTWO) &&
                     (game->board[i + 2][j] == PLAYERTWO) && (game->board[i + 3][j] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We start at the possible row positions for a "forward" diagonal.
    for (i = 0; i < NUM_ROWS - 3; i++) {

        // We start at the possible column positions. 
        for (j = 0; j < NUM_COLS - 3; j++) {

            if ((game->board[i][j] == PLAYERONE) && (game->board[i + 1][j + 1] == PLAYERONE) &&
                (game->board[i + 2][j + 2] == PLAYERONE) && (game->board[i + 3][j + 3] == PLAYERONE))

                return X_WINS;

            else if ((game->board[i][j] == PLAYERTWO) && (game->board[i + 1][j + 1] == PLAYERTWO) &&
                     (game->board[i + 2][j + 2] == PLAYERTWO) && (game->board[i + 3][j + 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We start at the possible row positions for a "backward" diagonal.
    for (i = 0; i < NUM_ROWS - 3; i++) {

        // Here are the possible column positions for a backwards diagonal.
        for (j = NUM_COLS - 1; j > 2; j--) {

            if ((game->board[i][j] == PLAYERONE) && (game->board[i + 1][j - 1] == PLAYERONE) &&
                (game->board[i + 2][j - 2] == PLAYERONE) && (game->board[i + 3][j - 3] == PLAYERONE))

                return X_WINS;

            else if ((game->board[i][j] == PLAYERTWO) && (game->board[i + 1][j - 1] == PLAYERTWO) &&
                     (game->board[i + 2][j - 2] == PLAYERTWO) && (game->board[i + 3][j - 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // See if there's an empty slot on the board.
    for (i = 0; i < NUM_COLS; i++)
        if (game->board[NUM_ROWS - 1][i] == EMPTY)
            return NOT_OVER;

    // If we get here, we have a CATS game.
    return CATS;

} // end check-status

char other(char player) {
    if (player == PLAYERONE)
        return PLAYERTWO;
    else
        return PLAYERONE;
}
