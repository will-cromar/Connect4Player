//
// Created by will on 3/28/16.
//

#ifndef CON4
#include "con4lib.h"
#endif

#define DEFAULT_C_VAL sqrt(2)

// Stores w_i and n_i
typedef struct weight {
    int wi, ni;
} weight;

int doBetter(const struct connect4 *game, int secondsleft);
int mcts();