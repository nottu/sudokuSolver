//
// Created by jperalta on 2/7/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "sudoku.h"

//Basic Sudoku Methods
Sudoku* newSudoku(byte len){
  Sudoku *s = (Sudoku*)malloc(sizeof(Sudoku));
  s->len = len;
  s->data = (byte**)malloc(sizeof(byte*) * s->len);
  s->data[0] = (byte*)malloc(sizeof(byte) * s->len * s->len);
  for (int i = 1; i < s->len; ++i) {
    s->data[i] = s->data[i - 1] + s->len;
  }
  return s;
}
Sudoku* readSudokuFromFile(const char* fileName, byte len){
  FILE *f = fopen(fileName, "r");
  if( f == NULL) {
    printf("Warning: Could not read file %s", fileName);
    return NULL;
  }
  Sudoku *s = newSudoku(len);
  byte sz = len * len;
  for (int i = 0; i < sz; ++i) {
    byte b;
    fscanf(f, "%hd", &b);
    s->data[0][i] = b;
  }
  fclose(f);
  return s;
}
void freeSudoku(Sudoku* s){
  free(s->data[0]);
  free(s->data);
  free(s);
}
void printSudoku(Sudoku* s){
  for (int i = 0; i < s->len; ++i) {
    for (int j = 0; j < s->len; ++j) {
      printf("%hd ", s->data[i][j]);
    }
    printf("\n");
  }
}

//Sudoku Evaluator