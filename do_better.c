//
// Created by will on 3/28/16.
//

#include "do_better.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int doBetter(const struct connect4 *game, int secondsleft) {
    const char me = game->whoseTurn;

    struct connect4 tempGame;
    memcpy(&tempGame, game, sizeof(struct connect4));


    return mcts(game);
}

int mcts(const struct connect4 *game) {
    return 0;
}