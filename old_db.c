//
// Created by will on 3/28/16.
//

#include "old_db.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMULATIONS 500000

#define DEFAULT_C_VAL sqrt(2)

// Stores w_i (wins after move i) and n_i (number of simulations after i)
typedef struct _proportion {
    int wi, ni;
} proportion;

double weightFunction(proportion weight, int t);
int greatestWeight(proportion weights[NUM_COLS], int t);
int old_fast_check_status(const struct connect4 *game);

int oldDoBetter(const struct connect4 *game, int secondsleft) {
    const char me = game->whoseTurn;

    struct connect4 tempGame;

    proportion weights[NUM_COLS];
    memset(weights, 0, sizeof(weights));

    srand(42);

    int i;
    for(i = 0; i < SIMULATIONS; i++) {
        memcpy(&tempGame, game, sizeof(struct connect4));
        char currPlayer = me;
        int statusCode = NOT_OVER, firstMove = -1;
        while ((statusCode = old_fast_check_status(&tempGame)) == NOT_OVER) {
            int move = rand() % NUM_COLS;
            if (not_valid(&tempGame, move))
                continue; // Try again

            if (firstMove == -1)
                firstMove = move;

            tempGame.board[get_row(&tempGame, move)][move] = currPlayer;
            currPlayer = other(currPlayer);
        }

        if (statusCode == X_WINS && me == PLAYERONE)
            weights[firstMove].wi++;
        else if (statusCode == O_WINS && me == PLAYERTWO)
            weights[firstMove].wi++;

        weights[firstMove].ni++;
    }

    return greatestWeight(weights, SIMULATIONS);
}

double weightFunction(proportion weight, int t) {
    return (double) weight.wi / weight.ni + sqrt(2.0) * sqrt(log(t)/weight.ni);
}

int greatestWeight(proportion weights[NUM_COLS], int t) {
    double greatest = 0;
    int bestMove = -1;

    int i;
    for(i = 0; i < NUM_COLS; i++) {
        double weight = weightFunction(weights[i], t);
        //printf("Column %d has a weight of %g\n", i, weight);
        if (weight > greatest) {
            greatest = weight;
            bestMove = i;
        }
    }

    return bestMove;
}

// The older and faster version of check_status from con4lib.h
// Credit to Arup Guha for this function
int old_fast_check_status(const struct connect4 *game) {

    int i, j;

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
}