/* Checks if the current player has 3wins available */
int g3_has3Wins(const struct connect4 *game) {
	int col;
	for (col = 0; col < NUM_COLS; col++) {
		int validRow = get_row(game, col);
		if (g3_is3Win(game, validRow, col, game->whoseTurn) return col;
	}
	
	return -1;
}
