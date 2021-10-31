#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#include <stdint.h>

#define DIM 9

typedef uint8_t t_sudoku[DIM][DIM];

void printSudoku(void);
void moveUser(int direction);
void writeNumber(int number);
void initSudoku(void);

#endif /*_SUDOKU_H_*/
