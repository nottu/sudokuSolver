#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"

int main(int argc, const char** argv) {
  if(argc < 2) return 1;
  // srand (time(NULL));
  Sudoku *s = readSudokuFromFile(argv[1], 9);
  printSudoku(s);

  constructiveSolution(s);
  freeSudoku(s);
  return 0;
}