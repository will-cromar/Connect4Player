#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef CON4
#define CON4
#endif // CON4

#define PLAYERONE 'X'
#define PLAYERTWO 'O'
#define EMPTY '_'
#define NUM_ROWS 6
#define NUM_COLS 7

#define INIT_TIME 180
#define DISPLAY_WAIT_TIME 1

#define CATS 0
#define X_WINS 1
#define O_WINS 2
#define NOT_OVER 3

struct connect4 {
    char board[NUM_ROWS][NUM_COLS];
    char whoseTurn;
};

void init_board(struct connect4 *game);

int not_valid(const struct connect4 *game, int curmove);

int get_row(const struct connect4 *game, int move);

void print_board(const struct connect4 *game);

int check_status(const struct connect4 *game);

char other(char player);
