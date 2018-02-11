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
  s->data = (byte**)malloc(sizeof(byte*) * len);
  s->data[0] = (byte*)malloc(sizeof(byte) * len * len);

  s->solution = (byte**)malloc(sizeof(byte*) * len);
  s->solution[0] = (byte*)malloc(sizeof(byte) * len * len);
  for (int i = 1; i < s->len; ++i) {
    s->data[i] = s->data[i - 1] + len;
    s->solution[i] = s->solution[i - 1] + len;
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
  free(s->solution[0]);
  free(s->solution);
  free(s);
}
void printSudoku(Sudoku* s){
  for (int i = 0; i < s->len; ++i) {
    for (int j = 0; j < s->len; ++j) {
      if(j % 3 ==0) printf("|");
      printf("%hd ", s->data[i][j]);
    }
    printf("\n");
    if( (i+1) % 3 == 0){
      printf("--------------------\n");
    }
  }
}

byte *getSortedIndexes( byte* arr, byte len, SortType s){
  byte *indexes = (byte*)malloc(sizeof(byte) * len);
  for (byte i = 0; i < len; ++i) indexes[i] = i;
  for (byte i = 0; i < len-1; ++i)
  {
    for (byte j = len -1; j > i; j--)
    {
      if(
         ( s == MAXSORT && arr[indexes[i]] < arr[indexes[j]]) || 
         ( s == MINSORT && arr[indexes[i]] > arr[indexes[j]]) ||
         ( arr[indexes[i]] == arr[indexes[j]] && rand() % 2) //"coin toss"
        ){
        byte t = indexes[i];
        indexes[i] = indexes[j];
        indexes[j] = t;
      }
    }
  }
  /** Test sorting **/
  for (int i = 0; i < len; ++i)
  {
    printf("%i : %i\n", indexes[i], arr[indexes[i]]);
  }
  return indexes;
}

byte** getSqOrder(Sudoku *s){
  byte *sqWeigth = (byte*)malloc(sizeof(byte) * s->len);
  // Indexes
  // | 0 | 1 | 2 |
  // | 3 | 4 | 5 |
  // | 6 | 7 | 8 |
  //we search for square with most items
  for (byte i = 0; i < s->len; ++i)
  {
    byte n_sq = 0;
    for (byte j = 0; j < s->len; ++j)
    {
      //note: 2/3 * 3 => 0, i/3 * 3 => 0,3,6
      if(s->data[j%3 + (i/3)*3][j/3 + (i*3)%9] != 0) n_sq++;
    }
    // printf("%i : %i\n",i, n_sq);
    sqWeigth[i] = n_sq;
  }
  byte *indexes = getSortedIndexes(sqWeigth, s->len, MAXSORT);
  byte **sqOrder = (byte**)malloc(sizeof(byte) * 2);
  sqOrder[1] = sqWeigth;
  sqOrder[0] = indexes;
  return sqOrder;
}

void fillSqPrbs(Sudoku *s, byte index, byte sz){
  // printf("%i : %i : %i\n", index, sz, s->len - sz);
  byte lsz = s->len - sz;
  byte **nums = (byte**) malloc(sizeof(byte) * lsz);
  nums[0] = (byte*) malloc(sizeof(byte) * lsz * lsz);

  byte sqrow = (index/3)*3;
  byte sqcol = (index*3)%9;
  // for each free index, set nums that can be put
  byte x = 0;
  for (byte i = 0; i < s->len; ++i){
    byte i_idx = i%3 + sqrow, j_idx = i/3 + sqcol;
    byte num = s->data[i_idx][j_idx];
    if( !num ){
      // printf("(%i, %i) : %i ", i_idx, j_idx, num);
      for (int j = 0; j < lsz; ++j)
      {
        // nums[x++][j] = num;
      }
    }
  }
  free(nums[0]);
  free(nums);
}

void constructiveSolution(Sudoku* s){
  byte **sqOrder = getSqOrder(s);
  for (int i = 0; i < s->len; ++i)
  {
    byte index = sqOrder[0][i], weigth = sqOrder[1][index];
    // printf("INDEX %i :\t", index);
    fillSqPrbs(s, index, weigth);
  }
  free(sqOrder[0]);
  free(sqOrder[1]);
  free(sqOrder);
}
//Sudoku Evaluator