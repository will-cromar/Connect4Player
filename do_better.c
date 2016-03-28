//
// Created by will on 3/28/16.
//

#include "do_better.h"
#include <stdio.h>
#include <stdlib.h>

int doBetter(const struct connect4 *game, int secondsleft) {
    /*int i;
    for(i = 0; i < NUM_COLS; i++) {
        if (game->board[NUM_ROWS-1][i] == '_') {
            return i;
        }
    }*/

    return mcts();
}

int mcts() {
    // Keep a weight for each possible move
    weight branchWeights[NUM_COLS];

    // More coming soon...

    return 0;
}
