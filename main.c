#include "sudoku.h"

int main(int argc, const char** argv) {
  if(argc < 2) return 1;
  Sudoku *s = readSudokuFromFile(argv[1], 9);
  printSudoku(s);
  freeSudoku(s);
  return 0;
}