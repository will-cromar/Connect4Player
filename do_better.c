//
// Created by will on 3/28/16.
//

#include "do_better.h"

#include <string.h>

#define SIMULATIONS 75000
#define NUM_MOVES NUM_ROWS*NUM_COLS

int doBetter(const struct connect4 *game, int secondsleft) {
    // Seed random with our magic number
    srand(42);

    int specialCase = handleSpecialCase(game);
    if (specialCase != -1) {
        puts("Moving based on special case");
        return specialCase;
    }

	/* // for testing
	int col;

	scanf("%d", &col);
	return col;
*/

    // Allocate our root
    MCnode *root = calloc(1, sizeof(MCnode));

    // Search the tree
    mcts(game, root);

    int *possibleMoves = get_possible_moves(game);
    int move = bestMove(root->scores, possibleMoves);

    // Clean up
    freeMCTree(root);
    free(possibleMoves);

    return move;
}

// Upper confidence bound formula
double UCB(int w_i, int n_i, int t) {
    // Offset to ensure all prior probabilities are > 0
    // Also avoids NaNs
    w_i = max(w_i, 1);
    n_i = max(n_i, 1);
    t = max(t, 1);

    return (double) w_i / n_i + DEFAULT_C_VAL * sqrt(log(t)/n_i);
}

// Determine the best move, given the scores, the number of simulations
int bestMove(proportion *scores, int *possibleMoves) {
    double greatest = 0;
    int bestMove = -1;

    int i;
    for(i = 0; i < NUM_COLS; i++) {
        double weight = (double) scores[i].w_i / max(scores[i].n_i, 1);
        //printf("Column %d has a weight of %g\n", i, weight);
        if (possibleMoves[i] == 1 && weight > greatest) {
            greatest = weight;
            bestMove = i;
        }
    }

    return bestMove;
}

// Weight paths based on some function of their score
// Returns a probability distribution
void computeWeightedProbs(double *probabilities, proportion *scores, int t) {
    double numerators[NUM_COLS];
    double denominator = 0;

    int i;
    for (i = 0; i < NUM_COLS; i++) {
        numerators[i] = UCB(scores[i].w_i, scores[i].n_i, t);
        denominator += numerators[i];
    }

    for(i = 0; i < NUM_COLS; i++)
        probabilities[i] = numerators[i] / denominator;
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

// Perform a Monte-Carlo Tree Search
void mcts(const struct connect4 *game, MCnode *root) {
    // For concision
    const char me = game->whoseTurn;
    //const int winCondition = me == PLAYERONE ? X_WINS : O_WINS;

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
        char winner = 0;
        while (winner == 0) {
            if (!movesAvailable(&tempGame))
                break;

            double probabilities[NUM_COLS];
            //const char currPlayer = tempGame.whoseTurn;

            // Determine which probabilities to use
            if (tempGame.whoseTurn == me)
                computeWeightedProbs(probabilities, current->scores, t);
            else
                computeUniformProbs(probabilities);

            // Use probabilities to select the next node
            int nextMove = handleSpecialCase(&tempGame);
            while (not_valid(&tempGame, nextMove))
                nextMove = chooseMove(probabilities);

            // Push the move onto the stack if this is our move
            if (tempGame.whoseTurn == me) {
                //printf("Stack pointer at %d\n", s);
                //print_board(&tempGame);
                moveStack[s] = nextMove;
                nodeStack[s++] = current;
            }

            // If the move would result in a winner or the game is CATS, break the loop
            int validRow = get_row(&tempGame, nextMove);
            if (is3Win(&tempGame, validRow, nextMove, tempGame.whoseTurn)) {
                winner = tempGame.whoseTurn;
            } else {
                // Otherwise, play the move and keep playing
                move(&tempGame, nextMove, tempGame.whoseTurn);
                tempGame.whoseTurn = other(tempGame.whoseTurn);
                current = getNextNode(current, nextMove);
            }
        }

        // Determine if we won
        int w = (winner == me);
        // Backprop the result of the game
        backpropogate(nodeStack, moveStack, s - 1, w);
    }

    return;
}

// Clean up the Monte-Carlo Tree
void freeMCTree(MCnode *root) {
    if (root == NULL)
        return;

    int i;
    for(i = 0; i < NUM_COLS; i++) {
        freeMCTree(root->moves[i]);
    }

    free(root);
}

//Finds the max of two numbers
inline int max(int a, int b) {
    return a < b ? a : b;
}

/* 
 * Handles the cases where pieces have only been played in the middle, 
 * There is a 3 in a row on the board to be blocked, or if there is
 * A two in a row that will result in 2 different places to win on the board,
 * and it is safe to block that 2 in a row from happening, or it is safe to start it
 */
 
int handleSpecialCase(const struct connect4 *game) {
    // Play the center if there are no other moves
    if (!not_valid(game, 3)) {
		//isCrazy checks if there are moves in other columns;
   		if (!isCrazy(game)) return 3; 
	}
	
	//Continue to checking for 3wins and 2wins if you shouldn't play the center
		
    int col;
	int didTheyWin = -1;

	//determines who is who for each of the pieces based on whose turn it is
	char us = game->whoseTurn;
	
	//they have to be the opposite of our piece, so set them to the opposite of us
	char them = other(us);

	//Check all of the columns for if we win by playing in the column or if we block their win
	for (col = 0; col < NUM_COLS; col++) {
		//finds the row you will be playing in if you select that column
		int validRow = get_row(game, col);
		//If you can win there, play there!
		if (is3Win(game, validRow, col, us)) {
			return col;
		}
		//Otherwise, keep track of where to block the opponent if they have a 3win
		if (is3Win(game, validRow, col, them)) didTheyWin = col;
	}
	
	//If you found a column you have to block, block it.
	if (didTheyWin != -1) return didTheyWin; 

	//If there are no 3wins, check for 2wins

	//Check in each col if there is a 2win
	for (col = 0; col < NUM_COLS; col++) {
		int validRow = get_row(game, col);
		
		//if you have a 2win and you don't cause a 3win for the opponent, you should play there
		if (is2Win(game, validRow, col, us)) {
			if (isSafe(game, validRow, col, us)) {
				return col;
			}
		}
/*
		//If your opponent has a 2win and its safe to block them, do it
		if (is2Win(game, validRow, col, them)) {
			if (isSafe(game, validRow, col, us)) {
				didTheyWin = col;
			}
		}
*/
	}
	

	//returns either -1 or the column your opponent had a 2win in
	return didTheyWin;

}

/* Finds out if there is pieces in anywhere other than the center */
int isCrazy(const struct connect4 *game) {
    int i;
	//Checks each column to see if there is a piece in the bottom row
    for (i = 0; i < NUM_COLS; i++) {
    	if (i != 3 && game->board[0][i] != EMPTY)
        	return 1;
    }
	//if there isnt, then you good fam
	return 0;
}

/* 
 * Given a location on the board and a piece, it goes and checks in each direction
 * if there are 3 pieces in a row that you should block
 */
int is3Win(const struct connect4 *game, int row, int col, char piece) {
    const int DXDYLENGTH = 8;
	char currPos = game->board[row][col];
	
	int i = 0;
	
	//If you can play in the spot given, check in each direction if you have a 3win
	if (currPos == EMPTY && canMove(game, row, col)) 
		for (i = 0; i < DXDYLENGTH; i++) 
			if (is3WinDirection(game, row, col, i, piece)) return 1;
			
	//Return 0 if you didnt find one
	return 0;
}

/* Given a direction and an original position, it will tell you if you win there */
int is3WinDirection(const struct connect4 *game, int row, int col, int dir, char piece) {
    const int DX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int DY[] = {-1, 1, 0, -1, 1, 0, -1, 1};
	
	//Count in the direction and the direction's inverse the number of pieces in a row around your blank
	int check3 = countOutwards(game, row, col, DX[dir], DY[dir], piece) + countOutwards(game, row, col, -DX[dir], -DY[dir], piece);
	
	//If you managed to get more than 3 in a row, its a valid play
	return check3 >= 3;
}

/* Recursively count the number of pieces in a direction given from a position */
int countOutwards(const struct connect4 * game, int row, int col, int dx, int dy, char piece) {
	//Where you are moving to
	int newRow = row + dy;
	int newCol = col + dx;
	//Stop when you hit the edge of the board or you have found a blank
	if (!isOnBoard(newRow, newCol) || game->board[newRow][newCol] != piece) return 0;
	//Continue onwards otherwise
	return 1 + countOutwards(game, newRow, newCol, dx, dy, piece); // really cool recursion :'''''''''''')
}


/* Checks to see if at the current location you have a 2win */
int is2Win(const struct connect4 *game, int row, int col, char piece) {
	const int DXDYLENGTH = 8;
	char currPos = game->board[row][col];
	int i = 0;
	
	//If you can play in the spot given, check in each direction if you have a 3win
	if (currPos == EMPTY && canMove(game, row, col)) 
		for (i = 0; i < DXDYLENGTH; i++) 
			if (is2WinDirection(game, row, col, i, piece)) return 1;
			
	//Return 0 if you didnt find one
	return 0;

}

/* Checks to see if at the current location and direction you have a 2win */
int is2WinDirection(const struct connect4 *game, int row, int col, int dir, char piece) {
	const int DX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
	const int DY[] = {-1, 1, 0, -1, 1, 0, -1, 1};
	
	//We are looking at the 5 pieces around us to see if we find our pattern for a 2win
	// (The pattern is _XX__, where index 3 is where you are moving, and you are able to play on all blanks present)
	char pieces[5];
	int i;
	
	//Look in each of the 5 slots in the direction around our piece and put that into our pieces
	//array which we are using to see if the pattern we want is present
	for (i = 0; i < 5; i++) {
		//Find the slot on the board 
		int newRow = row + (i - 3) * DY[dir];
		int newCol = col + (i - 3) * DX[dir];
		
		//Checks to see if the location is on the board, and if it is it puts it into pieces	
		if (isOnBoard(newRow, newCol)) {
			pieces[i] = game->board[newRow][newCol];
			//For blanks, you have to make sure you can move in each blank location
			//Here, i check if each blank is a valid location to move
			//If it is, then I switch the char to 'Y', otherwise its 'N'
			if (pieces[i] == '_') {
				if (canMove(game, newRow, newCol)) pieces[i] = 'Y';
				else pieces[i] = 'N';
			}
		}
		else {
			//set it to something that lets you know its off the board
			//'R' FOR RIP
			pieces[i] = 'R';
		}
	}

	//Now you have to check if your pattern holds
	return pieces[0] == 'Y' && pieces[1] == piece && pieces[2] == piece && pieces[3] == 'Y' && pieces[4] == 'Y';
}


/* Checks to see if placing a piece down causes a loss in the game */
int isSafe(const struct connect4 *game, int row, int col, char piece) {
	//Copies over the board
	struct connect4 * testBoard = copy(game);
	
	//Adds in your piece in the location listed
	testBoard->board[row][col] = piece;
	
	//Find out if they have a win if you put your piece in the location listed	
	char them = other(piece);
    // Assume safe if the opponent cannot play above us. Otherwise, check for 3-win
	int ret = !inbounds(row, col) ? 1 : !is3Win(testBoard, row+1, col, them);
	
	//free the copy of the board and return
	free(testBoard);
	
	return ret;
}

//Bool to see if a move is on the board
int isOnBoard(int i, int j) {
	return i >= 0 && j >= 0 && j < NUM_COLS && i < NUM_ROWS;
}

//Checks to see if a move is a valid place to move
int canMove(const struct connect4 *game, int i, int j) {
	if (isOnBoard(i, j)) {
		if (isOnBoard(i - 1, j) && game->board[i - 1][j] != EMPTY)
			return 1;
		if (!isOnBoard(i - 1, j))
			return 1; 
	}
	return 0;
}

// Returns 1 if there is at least one more available move
int movesAvailable(const struct connect4 *game) {
    int i;
    for (i = 0; i < NUM_COLS; i++) {
        if (game->board[NUM_ROWS - 1][i] == EMPTY)
            return 1;
    }

    return 0;
}
