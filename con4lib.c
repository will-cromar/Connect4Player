#ifndef CON4
#include "con4lib.h"
#endif

// Initializes the game so that it's ready to begin.
void init_board(struct connect4 *game) {

     int i, j;

     // Make each game square empty.
     for (i=0; i<NUM_ROWS; i++)
         for (j=0; j<NUM_COLS; j++)
             game->board[i][j] = EMPTY;

     // Player one gets to go first!
     game->whoseTurn = PLAYERONE;
}

// Returns 1 iff curmove is NOT a valid column to place a piece in game.
// Returns 0 otherwise.
int not_valid(const struct connect4 *game, int curmove) {

     // These columns don't exist on the board.
     if (curmove < 0 || curmove >= NUM_COLS)
         return 1;

     // Here we check to see if the column is full.
     else if (game->board[NUM_ROWS-1][curmove] != EMPTY)
         return 1;

     // Otherwise, we're okay.
     return 0;
}

// Pre-condition: move MUST BE a valid column to move in game.
// Returns the row number of where a piece would be placed at this point in
// the game in the column move.
int get_row(const struct connect4 *game, int move) {

    int i = NUM_ROWS;

    // Move through each row, until you hit a piece.
    while (i > 0 && game->board[i-1][move] == EMPTY) i--;

    // Return this row number.
    return i;
}

// Returns an array such that array[i] = 1 if moving
// in column i is possible and array[i] = 0 if moving
// in column i is not possible.
int* get_possible_moves(const struct connect4 *game) {

    // Allocate array and store those moves.
    int* list = malloc(sizeof(int)*NUM_COLS);
    int i;
    for (i=0; i<NUM_COLS; i++)
        list[i] = (get_row(game, i) < NUM_ROWS);

    // Return the list.
    return list;
}

void move(struct connect4* game, int move, char player) {
    game->board[get_row(game, move)][move] = player;
}


// Prints out a representation of the current board
void print_board(const struct connect4 *game) {

    int i,j;

    printf("Here is the game board:\n\n");

    // Print the column numbers above the board.
    for (i=0; i<NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");

    // Go through each row of the board.
    for (i=NUM_ROWS-1; i>=0; i--) {

        for (j=0; j<NUM_COLS; j++)
            printf("%c ",game->board[i][j]);
        printf("\n");
    }

    // Print the column numbers below the board.
    for (i=0; i<NUM_COLS; i++)
        printf("%d ", i);
    printf("\n");
}

int inbounds(int x, int y) {
    return x >= 0 && x < NUM_COLS && y >= 0 && y < NUM_ROWS;
}

// Returns the number of items in the game board in the set of squares,
// starting at row y, column x moving in the direction (dx, dy) for len
// number of squares, that are equal to piece.
int count_char(const struct connect4* game, int x, int y, int dx, int dy, int len, char piece) {
    int i, res = 0;
    for (i=0; i<len; i++)
        if (game->board[x+dx*i][y+dy*i] == piece)
            res++;
    return res;
}

// Returns the current status of game.
int check_status(const struct connect4 *game) {

    int dir,i,j;

    int DX[] = {0,1,1,1}; // cols
    int DY[] = {-1,-1,0,1}; // rows

    // Look in each possible direction.
    for (dir=0; dir<NUM_DIR; dir++) {

        // Try each starting spot.
        for (i=0; i<NUM_COLS; i++) {
            for (j=0; j<NUM_ROWS; j++) {

                // Calculate the corresponding ending point.
                int lastx = i + DX[dir]*(WIN_STREAK-1);
                int lasty = j + DY[dir]*(WIN_STREAK-1);

                // Not a possible streak.
                if (!inbounds(lastx, lasty)) continue;

                if (count_char(game, j, i, DY[dir], DX[dir], WIN_STREAK, PLAYERONE) == WIN_STREAK)
                    return X_WINS;
                if (count_char(game, j, i, DY[dir], DX[dir], WIN_STREAK, PLAYERTWO) == WIN_STREAK)
                    return O_WINS;
            }
        }
    }

    // See if there's an empty slot on the board.
    for (i=0; i<NUM_COLS; i++)
        if (game->board[NUM_ROWS-1][i] == EMPTY)
            return NOT_OVER;

    // If we get here, we have a CATS game.
    return CATS;

} // end check-status

char other(char player) {
     if (player == PLAYERONE)
         return PLAYERTWO;
     else
         return PLAYERONE;
}

// Returns a pointer to a deep copy of game.
struct connect4* copy(const struct connect4* game) {
    struct connect4* res = malloc(sizeof(struct connect4));
    int i,j;
    for (i=0; i<NUM_ROWS; i++)
        for (j=0; j<NUM_COLS; j++)
            res->board[i][j] = game->board[i][j];
    res->whoseTurn = game->whoseTurn;
    return res;
}
