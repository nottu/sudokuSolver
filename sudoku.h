//
// Created by jperalta on 2/7/18.
//

#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "util.h"

typedef struct {
  byte **data;
  byte **solution;
  byte len; //sudoku are square, max val is equal to length - 1
} Sudoku;

Sudoku* readSudokuFromFile(const char* fileName, byte len);
void freeSudoku(Sudoku* s);
void printSudoku(Sudoku* s);
void printSudokuSol(Sudoku* s);

void constructiveSolution(Sudoku* s);

int evalSolution(Sudoku *s);
#endif //SUDOKU_SUDOKU_H
