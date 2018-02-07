//
// Created by jperalta on 2/7/18.
//

#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H
#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef unsigned short byte;
#endif

typedef struct {
  byte **data;
  byte len; //sudoku are square, max val is equal to length - 1
} Sudoku;

Sudoku* readSudokuFromFile(const char* fileName, byte len);
void freeSudoku(Sudoku* s);
void printSudoku(Sudoku* s);
#endif //SUDOKU_SUDOKU_H
