#ifndef __GAME_H__
#define __GAME_H__

#define ROW 5 //棋盘长宽
#define COL 5

void menu();
void game(int x);
void chessboard(char *x, int row, int col);
int estimate(char *x, char symbol, int row, int col);
void circulation(char *x, int row, int col);
void adaptive_circulation(char *x, int row, int col);
void usual_computer(char *x, char symbol, int row, int col);
// void advance_computer(char *x, char symbol, int row, int col);
void chessboard_computer(char *x, int row, int col);

#endif