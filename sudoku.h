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
  byte err;
} Sudoku;

Sudoku* readSudokuFromFile(const char* fileName, byte len);
void freeSudoku(Sudoku* s);
void printSudoku(Sudoku* s);
void printSudokuSol(Sudoku* s);

byte getNumFreeSpaces(Sudoku *s);
byte constructiveSolution(Sudoku* s);
void localSearchSolution(Sudoku *s);
typedef struct {
  byte square;
  byte from;
  byte to;
  byte weigth;
} SudokuSearchItem;


int evalSolution(Sudoku *s);
#endif //SUDOKU_SUDOKU_H
