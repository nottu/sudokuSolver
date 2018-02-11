#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"

int main(int argc, const char** argv) {
  if(argc < 2) return 1;
  int a = time(NULL);
  printf("%d\n", a);
  srand (a);
  // srand (1518347779);
  Sudoku *s = readSudokuFromFile(argv[1], 9);
  // printSudoku(s);
  // printf("*********************\n");
  // printf("Free Spaces: Before %i\t After %i\n", 
  //   getNumFreeSpaces(s), constructiveSolution(s));
  constructiveSolution(s);
  // printSudoku(s);
  // printf("*********************\n");
  // printSudokuSol(s);
  int c = evalSolution(s);
  printf("%i\n", c);
  printf("*********************\n");
  localSearchSolution(s);
  // printSudokuSol(s);
  c = evalSolution(s);
  printf("%i\n", c);
  freeSudoku(s);
  return 0;
}