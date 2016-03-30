//
// Created by will on 3/28/16.
//

#ifndef CON4
#include "con4lib.h"
#endif

#define DEFAULT_C_VAL sqrt(2)

// Stores w_i (wins after move i) and n_i (number of simulations after i)
typedef struct _proportion {
    int wi, ni;
} proportion;

int doBetter(const struct connect4 *game, int secondsleft);
int mcts(const struct connect4 *game);
int search(proportion *record);