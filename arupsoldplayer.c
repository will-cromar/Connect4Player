#include "arupsoldplayer.h"

// team is who the this player is moving for.
int score(char board[6][7], char team) {

    int i, j;
    int k, l;
    int total;

    char comp = PLAYERTWO, other = PLAYERONE;

    // Switch these if we need to.
    if (team == PLAYERONE) {
        comp = PLAYERONE;
        other = PLAYERTWO;
    }

    total = 0;

    // Iterate through starting rows for each square with a full neighborhood.
    for (i = 1; i < 5; i++) {

        // Iterate through starting columns for each square with a full neighborhood
        for (j = 1; j < 6; j++) {

            // See if this square is team 'O'
            if (board[i][j] == comp) {

                // Go through the neighborhood of squares.
                for (k = -1; k < 2; k++) {

                    for (l = -1; l < 2; l++) {

                        // Two points for an adjacent 'O'
                        if (board[i + k][j + l] == comp)
                            total = total + 2;

                            // One point for an adjacent blank.
                        else if (board[i + k][j + l] == EMPTY)
                            total = total + 1;
                    }
                }

            }

                // Work out for team 'X'
            else if (board[i][j] == other) {

                // Go through the neighborhood of squares again. 
                for (k = -1; k < 2; k++) {

                    for (l = -1; l < 2; l++) {

                        if (board[i + k][j + l] == other)
                            total = total - 2;
                        else if (board[i + k][j + l] == EMPTY)
                            total = total - 1;
                        else if (board[i + k][j + 1] == comp)
                            total = total + 1;
                    }
                }

            } // end if-else if
        } // end j
    } // end i

    // We also add/subtract some points based on what's going on, on the bottom
    // row. (I think this is retarded, but apparently when I coded this up 
    // back in 1999 I didn't.)
    for (i = 0; i < 7; i++) {

        if (board[0][i] == other) {

            if (board[0][i + 1] == other)
                total = total - 2;
            else if (board[0][i + 1] == comp)
                total = total + 1;

            if (board[0][i - 1] == other)
                total = total - 2;
            else if (board[0][i - 1] == comp)
                total = total + 1;
        }
    }

    return total;
}


int arup_move(const struct connect4 *game, int secondsleft) {

    int i, j;
    int status;
    int move, randmove;
    int brdscore, maxscore;
    char saveboard[NUM_ROWS][NUM_COLS];

    // Here's a copy of the board.
    for (i = 0; i < NUM_ROWS; i++)
        for (j = 0; j < NUM_COLS; j++)
            saveboard[i][j] = game->board[i][j];

    // Look for a win on this particular move.
    for (i = 0; i < 7; i++) {

        saveboard[get_row(game, i)][i] = game->whoseTurn;

        status = chk_status(saveboard);

        if (game->whoseTurn == PLAYERONE && status == X_WINS)
            return i;
        if (game->whoseTurn == PLAYERTWO && status == O_WINS)
            return i;

        saveboard[get_row(game, i)][i] = EMPTY;
    }

    // Look to block a potential win.
    for (i = 0; i < 7; i++) {

        saveboard[get_row(game, i)][i] = other(game->whoseTurn);
        status = chk_status(saveboard);

        if (game->whoseTurn == PLAYERTWO && status == X_WINS)
            return i;
        if (game->whoseTurn == PLAYERONE && status == O_WINS)
            return i;

        saveboard[get_row(game, i)][i] = '_';
    }

    // Look at the bottom row to see if we have two open sides.
    for (i = 0; i < 4; i++) {

        if ((saveboard[0][i] == '_') && (saveboard[0][i + 1] == game->whoseTurn) &&
            (saveboard[0][i + 2] == game->whoseTurn) && (saveboard[0][i + 3] == '_')) {

            // Not quite sure what's going on here...I leave one empty spot
            // instead of going for three in a row in these first two cases.
            if (i == 0)
                return 4;
            else if (i == 3)
                return 4;

                // Here I go for three in a row...
            else
                return i;
        }

    } // end for i=0 to 4

    maxscore = -1000;

    // Try out each move if there's no way to win or lose in one turn and
    // return the best scored move.
    for (i = 0; i < 7; i++) {

        if (saveboard[NUM_ROWS - 1][i] != EMPTY) continue;

        saveboard[get_row(game, i)][i] = game->whoseTurn;
        brdscore = score(saveboard, game->whoseTurn);

        if (brdscore > maxscore) {

            maxscore = brdscore;
            move = i;
        }

        saveboard[get_row(game, i)][i] = '_';
    }

    return move;

}

// Returns the current status of game.
int chk_status(char board[][NUM_COLS]) {

    int i, j, k;

    // We go through each row, to look for a horizontal win.
    for (j = 0; j < NUM_ROWS; j++) {

        // We iterate through the possible column starting positions of four
        // consecutive winning pieces.
        for (i = 0; i < NUM_COLS - 3; i++) {

            if ((board[j][i] == PLAYERONE) && (board[j][i + 1] == PLAYERONE) &&
                (board[j][i + 2] == PLAYERONE) && (board[j][i + 3] == PLAYERONE))

                return X_WINS;

            else if ((board[j][i] == PLAYERTWO) && (board[j][i + 1] == PLAYERTWO) &&
                     (board[j][i + 2] == PLAYERTWO) && (board[j][i + 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We go through each column, to look for a vertical win.
    for (j = 0; j < NUM_COLS; j++) {

        // We iterate through possible row starting positions of four 
        // consecutive winning pieces.
        for (i = 0; i < NUM_ROWS - 3; i++) {

            if ((board[i][j] == PLAYERONE) && (board[i + 1][j] == PLAYERONE) &&
                (board[i + 2][j] == PLAYERONE) && (board[i + 3][j] == PLAYERONE))

                return X_WINS;

            else if ((board[i][j] == PLAYERTWO) && (board[i + 1][j] == PLAYERTWO) &&
                     (board[i + 2][j] == PLAYERTWO) && (board[i + 3][j] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We start at the possible row positions for a "forward" diagonal.
    for (i = 0; i < NUM_ROWS - 3; i++) {

        // We start at the possible column positions. 
        for (j = 0; j < NUM_COLS - 3; j++) {

            if ((board[i][j] == PLAYERONE) && (board[i + 1][j + 1] == PLAYERONE) &&
                (board[i + 2][j + 2] == PLAYERONE) && (board[i + 3][j + 3] == PLAYERONE))

                return X_WINS;

            else if ((board[i][j] == PLAYERTWO) && (board[i + 1][j + 1] == PLAYERTWO) &&
                     (board[i + 2][j + 2] == PLAYERTWO) && (board[i + 3][j + 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // We start at the possible row positions for a "backward" diagonal.
    for (i = 0; i < NUM_ROWS - 3; i++) {

        // Here are the possible column positions for a backwards diagonal.
        for (j = NUM_COLS - 1; j > 2; j--) {

            if ((board[i][j] == PLAYERONE) && (board[i + 1][j - 1] == PLAYERONE) &&
                (board[i + 2][j - 2] == PLAYERONE) && (board[i + 3][j - 3] == PLAYERONE))

                return X_WINS;

            else if ((board[i][j] == PLAYERTWO) && (board[i + 1][j - 1] == PLAYERTWO) &&
                     (board[i + 2][j - 2] == PLAYERTWO) && (board[i + 3][j - 3] == PLAYERTWO))

                return O_WINS;
        }
    }

    // See if there's an empty slot on the board.
    for (i = 0; i < NUM_COLS; i++)
        if (board[NUM_ROWS - 1][i] == EMPTY)
            return NOT_OVER;

    // If we get here, we have a CATS game.
    return CATS;

} // end check-status
