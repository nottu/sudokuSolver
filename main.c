#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"

int main(int argc, const char** argv) {
  if(argc < 3) return 1;
  int n_execs = atoi(argv[2]);
  int a = time(NULL);
  // printf("%d\n", a);
  srand (a);

  int n_suc = 0;

  for (int i = 0; i < n_execs; ++i)
  {
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
    printf("Constructive: %i\n", c);
    // printf("*********************\n");
    localSearchSolution(s);
    // printSudokuSol(s);
    c = evalSolution(s);
    printf("Iterative %i\n", c);
    freeSudoku(s);
    if(c == 0) n_suc++;
   }
   printf("EXITOS %i\n", n_suc);
  return 0;
}