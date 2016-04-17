//
// Created by will on 3/28/16.
//

#ifndef CON4
#include "con4lib.h"
#endif

#define g3_NUM_MOVES NUM_ROWS*NUM_COLS
#define g3_CENTER NUM_COLS / 2

#define g3_DEFAULT_C_VAL sqrt(2)
#define g3_SIMULATIONS 75000

// Stores w_i (wins after move i) and n_i (number of simulations after i)
typedef struct _g3_proportion {
    int w_i, n_i;
} g3_proportion;

// Node of a Monte Carlo tree
typedef struct _g3_MCnode {
    struct _g3_proportion scores[NUM_COLS];
    struct _g3_MCnode *moves[NUM_COLS];
} g3_MCnode;

// Functions for move selection
int g3_move(const struct connect4 *game, int secondsleft);
int g3_handleSpecialCase(const struct connect4 *game);
void g3_mcts(const struct connect4 *game, g3_MCnode *root);
int g3_bestMove(g3_proportion *scores, int *possibleMoves);

// Support functions for Monte-Carlo Tree Search
double g3_UCB(int w_i, int n_i, int t);
void g3_computeWeightedProbs(double *probabilities, g3_proportion *scores, int t);
void g3_computeUniformProbs(double *probabilities);
int g3_chooseMove(double *probabilities);
void g3_backpropogate(g3_MCnode **nodeStack, int *moveStack, int s, int w);
g3_MCnode *g3_getNextNode(g3_MCnode *node, int move);
void g3_freeMCTree(g3_MCnode *root);

/* All tied to finding a 3Win */
int g3_is3Win(const struct connect4 *game, int row, int col, char piece);
int g3_is3WinDirection(const struct connect4 *game, int row, int col, int dir, char piece);
int g3_countOutwards(const struct connect4 *game, int row, int col, int dx, int dy, char piece);

/* All tied to finding a 2Win */
int g3_is2Win(const struct connect4 *game, int row, int col, char piece);
int g3_is2WinDirection(const struct connect4 *game, int row, int col, int dir, char piece);

/* Bools Used in code */
int g3_isSafe(const struct connect4 *game, int row, int col, char piece);
int g3_canMove(const struct connect4 *game, int i, int j);
int g3_isOnBoard(int i, int j);
int g3_isCrazy(const struct connect4 *game);
int g3_movesAvailable(const struct connect4 *game);

// Something that should already be in C
int g3_max(int a, int b);