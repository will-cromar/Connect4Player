//
// Created by will on 3/28/16.
//

#ifndef CON4
#include "con4lib.h"
#endif

#define NUM_MOVES NUM_ROWS*NUM_COLS
#define CENTER NUM_COLS / 2

#define DEFAULT_C_VAL sqrt(2)
#define SIMULATIONS 40000

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
int bestMove(proportion *scores, int *possibleMoves);
double UCB(int w_i, int n_i, int t);
void mcts(const struct connect4 *game, MCnode *root);
void computeWeightedProbs(double *probabilities, proportion *scores, int t);
void computeUniformProbs(double *probabilities);
void freeMCTree(MCnode *root);
int handleSpecialCase(const struct connect4 *game);
int max(int a, int b);

int handleSpecialCase(const struct connect4 *game);

/* All tied to finding a 3Win */
int is3Win(const struct connect4 *game, int row, int col, char piece);
int is3WinDirection(const struct connect4 *game, int row, int col, int dir, char piece);
int countOutwards(const struct connect4 * game, int row, int col, int dx, int dy, char piece);

/* All tied to finding a 2Win */
int is2Win(const struct connect4 *game, int row, int col, char piece);
int is2WinDirection(const struct connect4 *game, int row, int col, int dir, char piece);

/* Bools Used in code */
int isSafe(const struct connect4 *game, int row, int col, char piece);
int canMove(const struct connect4 *game, int i, int j);
int isOnBoard(int i, int j);
int isCrazy(const struct connect4 *game);
int isSafe(const struct connect4 *game, int row, int col, char piece);
int movesAvailable(const struct connect4 *game);
