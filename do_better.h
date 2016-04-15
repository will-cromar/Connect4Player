//
// Created by will on 3/28/16.
//

#ifndef CON4
#include "con4lib.h"
#endif

#define DEFAULT_C_VAL sqrt(2)


/* Labels the different types of wins that can be returned from the CheckLocation function */
typedef enum _WinType {
    OUR3WIN, THEIR3WIN, OUR2WIN, THEIR2WIN, NOWINS
} WinType;

// Stores w_i (wins after move i) and n_i (number of simulations after i)
typedef struct _proportion {
    int w_i, n_i;
} proportion;

// Node of a Monte Carlo tree
typedef struct _MCnode {
    struct _proportion scores[NUM_COLS];
    struct _MCnode *moves[NUM_COLS];
} MCnode;

int doBetter(const struct connect4 *game, int secondsleft);
int bestMove(proportion *scores, int t, int *possibleMoves);
double UCB(int w_i, int n_i, int t);
void mcts(const struct connect4 *game, MCnode *root);
void computeWeightedProbs(double *probabilities, proportion *scores, int t);
void computeUniformProbs(double *probabilities);
void freeMCTree(MCnode *root);
int fast_check_status(const struct connect4 *game);
int handleSpecialCase(const struct connect4 *game);

char Locations_CheckLocation(const struct connect4 *game, int row, int col);
int handleSpecialCase(const struct connect4 *game);
int isOnBoard(int i, int j);
int canMove(const struct connect4 *game, int i, int j);
char is3Win(const struct connect4 *game, int row, int col, int dir);

