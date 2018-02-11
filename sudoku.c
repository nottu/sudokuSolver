//
// Created by jperalta on 2/7/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sudoku.h"

//Basic Sudoku Methods
Sudoku* newSudoku(byte len){
  Sudoku *s = (Sudoku*)malloc(sizeof(Sudoku));
  s->len = len;
  s->data = allocByteArr(len);
  s->solution = allocByteArr(len);
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
  freeByteArr(s->data);
  freeByteArr(s->solution);
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
  // for (int i = 0; i < len; ++i)
  // {
  //   printf("%i : %i\n", indexes[i], arr[indexes[i]]);
  // }
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

byte *getNumsInSq(Sudoku *s, byte index, byte sz, byte* indexes){
  byte *nums = (byte*)malloc(sizeof(byte) * s->len);
  for (int i = 0; i < s->len; ++i) nums[i] = i+1;
  byte sqrow = (index/3)*3;
  byte sqcol = (index*3)%9;
  byte idx = 0;
  for (byte i = 0; i < s->len; ++i){
    byte i_idx = i/3 + sqrow, j_idx = i%3 + sqcol;
    byte num = s->data[i_idx][j_idx];
    if( num ){
      nums[num-1] = 0;
    } else {
      indexes[idx++] = i;
    }
  }
  byte *notUsed = (byte*)malloc(sizeof(byte) * sz);
  idx = 0;
  for (int i = 0; i < s->len; ++i)
  {
    if(nums[i]) notUsed[idx++] = nums[i];
  }
  free(nums);
  return notUsed;
}
// marks as 0 the nums already used in row (only "hard truths")
byte getNumsInRow(Sudoku *s, byte row, byte* nums, byte sz){
  byte cnt = 0;
  for (byte i = 0; i < s->len; ++i){
    if(s->data[row][i]){
      for (byte j = 0; j < sz; ++j)
      {
        if(nums[j] == s->data[row][i]){
          nums[j] = 0;
          cnt++;
          break;
        }
      }
    }
  }
  return sz - cnt;
}
// marks as 0 the nums already used in column (only "hard truths")
byte getNumsInCol(Sudoku *s, byte col, byte* nums, byte sz){
  byte cnt = 0;
  for (byte i = 0; i < s->len; ++i){
    if(s->data[i][col]){
      for (byte j = 0; j < sz; ++j)
      {
        if(nums[j] == s->data[i][col]){
          nums[j] = 0;
          cnt++;
          break;
        }
      }
    }
  }
  return sz - cnt;
}

void fillSqPrbs(Sudoku *s, byte index, byte sz){
  sz = s->len - sz;
  byte* indexes = (byte*)malloc(sizeof(byte) * sz);
  byte* weigth = (byte*)malloc(sizeof(byte) * sz);
  byte* notUsed = getNumsInSq(s, index, sz, indexes);
  //stores possible nums for cell
  byte **nums = allocByteArr(sz);
  byte sqrow = (index / 3) * 3;
  byte sqcol = (index * 3) % 9;
  for (int i = 0; i < sz; ++i){
    memcpy(nums[i], notUsed, sizeof(byte) * sz);
    getNumsInRow(s, sqrow + (indexes[i] / 3), nums[i], sz);
    weigth[i] = getNumsInCol(s, sqcol + (indexes[i] % 3), nums[i], sz);
  }
  free(notUsed);
  printByteArr(nums, sz);

  free(indexes);
  free(weigth);
  freeByteArr(nums);
}

void constructiveSolution(Sudoku* s){
  byte **sqOrder = getSqOrder(s);
  #warning solo hace 1
  for (int i = 0; i < 1; ++i)
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