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

/*
 * Used to return the column positions of the two different types of win locations
 * The win with 3 in a row and 1 blank and the win in 2 turns, where you have two
 * pieces next to each other and two blanks on either side, and one side has one
 * extra blank. (If you play on the side with the 2 blanks, you end up having two ways
 * to win the next turn, either playing on the left or the right of the 3win you have
 * created. This will force a victory unless your opponent manages to win on their turn)
 */
typedef struct _Locations {
	int Our3Win;
	int Our2Win;
	int Their3Win;
	int Their2Win;
} Locations;


int doBetter(const struct connect4 *game, int secondsleft);
int bestMove(proportion *scores, int t, int *possibleMoves);
double UCB(int w_i, int n_i, int t);
void mcts(const struct connect4 *game, MCnode *root);
void computeWeightedProbs(double *probabilities, proportion *scores, int t);
void computeUniformProbs(double *probabilities);
void freeMCTree(MCnode *root);
int fast_check_status(const struct connect4 *game);
int handleSpecialCase(const struct connect4 *game);
/*
Locations * new_Locations(void);
WinType Locations_CheckLocation(const struct connect4 *game, int i, int j);
int CollectWinLocations(const struct connect4 *game);

int isOnBoard(int i, int j);
int canMove(const struct connect4 *game, int i, int j);
char is3Win(const struct connect4 *game, int *iPos, int *jPos);
char is2Win(const struct connect4 *game, int *iPos, int *jPos);
*/