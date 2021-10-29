#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include <stdint.h>

#define ROWS 9
#define COLS 9

typedef uint8_t t_sudoku[ROWS][COLS];

void printSudoku(void);
void moveUser(int direction);
void writeNumber(int number);

#endif /*_SUDOKU_H_*/
