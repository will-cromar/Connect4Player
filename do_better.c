//
// Created by will on 3/28/16.
//

#include "do_better.h"

#include <string.h>

#define SIMULATIONS 500000
#define NUM_MOVES NUM_ROWS*NUM_COLS

int doBetter(const struct connect4 *game, int secondsleft) {
    // Seed random with our magic number
    srand(42);

    // Allocate our root
    MCnode *root = calloc(1, sizeof(MCnode));

    // Search the tree
    mcts(game, root);


    int *possibleMoves = get_possible_moves(game);
    int move = bestMove(root->scores, SIMULATIONS, possibleMoves);

    // Clean up
    freeMCTree(root);
    free(possibleMoves);

    return move;
}

// Upper confidence bound formula
double UCB(int w_i, int n_i, int t) {
    // Offset to ensure all prior probabilities are > 0
    w_i += 1;
    n_i += 1;

    return (double) w_i / n_i + DEFAULT_C_VAL * sqrt(log(t)/n_i);
}

// Determine the best move, given the scores, the number of simulations
int bestMove(proportion *scores, int t, int *possibleMoves) {
    double greatest = 0;
    int bestMove = -1;

    int i;
    for(i = 0; i < NUM_COLS; i++) {
        double weight = UCB(scores[i].w_i, scores[i].n_i, t);
        //printf("Column %d has a weight of %g\n", i, weight);
        if (possibleMoves[i] == 1 && weight > greatest) {
            greatest = weight;
            bestMove = i;
        }
    }

    return bestMove;
}

void computeWeightedProbs(double *probabilities, proportion *scores, int t) {
    double numerators[NUM_COLS];
    double denominator = 0;

    int i;
    for (i = 0; i < NUM_COLS; i++) {
        numerators[i] = UCB(scores[i].w_i, scores[i].n_i, t);
        denominator += numerators[i];
    }

    for(i = 0; i < NUM_COLS; i++) probabilities[i] = numerators[i] / denominator;
}

// Every path is given an equal weight
void computeUniformProbs(double *probabilities) {
    int i;
    for (i = 0; i < NUM_COLS; i++)
        probabilities[i] = 1.0 / NUM_COLS;

    return;
}

// Backpropogate the result of a win
void backpropogate(MCnode **nodeStack, int *moveStack, int s, int w) {
    // Empty out the stack
    while (s >= 0) {
        int move = moveStack[s];
        nodeStack[s]->scores[move].n_i += 1; // Add 1 to the number of games played
        nodeStack[s]->scores[move].w_i += w; // Add a win if we won
        s--;
    }
}

// Get node->moves[move] if it exists, or create it, if necessary
MCnode *getNextNode(MCnode *node, int move) {
    if (node->moves[move] == NULL)
        node->moves[move] = calloc(1, sizeof(MCnode));

    if (node->moves[move] == NULL)
        puts("The operating system won't give me memory :(");

    return node->moves[move];
}

// Randomly choose a move based on probabilities
int chooseMove(double *probabilities) {
    // Choose a number from 0 through 1
    double roll = (double) rand() / RAND_MAX;
    double cumulativeProb = 0;

    int i;
    for (i = 0; i < NUM_COLS; i++) {
        cumulativeProb += probabilities[i];
        if (cumulativeProb >= roll) {
            return i;
        }
    }

    // This should never happen.
    return rand()%NUM_COLS;
}

void mcts(const struct connect4 *game, MCnode *root) {
    // For concision
    const char me = game->whoseTurn;
    const int winCondition = me == PLAYERONE ? X_WINS : O_WINS;

    // Memory to do scratch-work in
    struct connect4 tempGame;

    MCnode *nodeStack[NUM_MOVES];
    int moveStack[NUM_MOVES];
    int s = 0; // Stack pointer

    int t; // current number of simulations
    for (t = 0; t < SIMULATIONS; t++) {
        // Make a fresh copy of the game
        memcpy(&tempGame, game, sizeof(struct connect4));

        // Temp pointer to the current node in the tree
        MCnode *current = root;

        // Reset stack pointer
        s = 0;

        // Play moves until the hypothetical game ends
        int statusCode = NOT_OVER;
        while ((statusCode = fast_check_status(&tempGame)) == NOT_OVER) {
            double probabilities[NUM_COLS];
            //const char currPlayer = tempGame.whoseTurn;

            // Determine which probabilities to use
            if (tempGame.whoseTurn == me)
                computeWeightedProbs(probabilities, current->scores, t);
            else
                computeUniformProbs(probabilities);

            // Use probabilities to select the next node
            int move = -1;
            while (not_valid(&tempGame, move))
                move = chooseMove(probabilities);

            // Push the move onto the stack if this is our move
            if (tempGame.whoseTurn == me) {
                //printf("Stack pointer at %d\n", s);
                //print_board(&tempGame);
                moveStack[s] = move;
                nodeStack[s++] = current;
            }

            tempGame.board[get_row(&tempGame, move)][move] = tempGame.whoseTurn;
            tempGame.whoseTurn = other(tempGame.whoseTurn);
            current = getNextNode(current, move);
        }

        // Determine if we won
        int w = (statusCode == winCondition);
        // Backprop the result of the game
        backpropogate(nodeStack, moveStack, s - 1, w);
    }

    return;
}

void freeMCTree(MCnode *root) {
    if (root == NULL)
        return;

    int i;
    for(i = 0; i < NUM_COLS; i++) {
        freeMCTree(root->moves[i]);
    }

    free(root);
}


// The older and faster version of check_status from con4lib.h
// Credit to Arup Guha for this function
int fast_check_status(const struct connect4 *game) {

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