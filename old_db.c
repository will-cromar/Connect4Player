//
// Created by will on 3/28/16.
//

#include "old_db.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIMULATIONS 1000000

#define DEFAULT_C_VAL sqrt(2)

// Stores w_i (wins after move i) and n_i (number of simulations after i)
typedef struct _proportion {
    int wi, ni;
} proportion;

double weightFunction(proportion weight, int t);
int greatestWeight(proportion weights[NUM_COLS], int t);

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
        while ((statusCode = check_status(&tempGame)) == NOT_OVER) {
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