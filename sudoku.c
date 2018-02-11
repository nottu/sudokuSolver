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

void printSudokuSol(Sudoku* s){
  for (int i = 0; i < s->len; ++i) {
    for (int j = 0; j < s->len; ++j) {
      if(j % 3 ==0) printf("|");
      printf("%d ", s->solution[i][j] | s->data[i][j]);
    }
    printf("\n");
    if( (i+1) % 3 == 0){
      printf("--------------------\n");
    }
  }
}
/** Sudoku Evaluator **/
int rowConflicts(Sudoku *s, byte row){
  int conflicts = 0;
  byte *nums = (byte *)malloc(sizeof(byte) * s->len);
  for (int i = 0; i < s->len; ++i) nums[i] = 0;
  for (byte j = 0; j < s->len; ++j){
    nums[(s->data[row][j] | s->solution[row][j]) - 1]++;
  }
  // printf("Row Items : %i\n", row+1);
  for (int i = 0; i < s->len; ++i)
  {
    nums[i] = nums[i] ? nums[i] - 1 : 0;
  }
  for (int i = 0; i < s->len; ++i)
  {
    byte val = s->data[row][i];
    if(val) nums[val - 1] *= 100;
  }
  for (int i = 0; i < s->len; ++i)
  {
    // printf("%i\t", nums[i]);
    conflicts += nums[i];
  }
  // printf("\t TOTAL:%i \n", conflicts);
  return conflicts;
}
int colConflicts(Sudoku *s, byte col){
  int conflicts = 0;
  byte *nums = (byte *)malloc(sizeof(byte) * s->len);
  for (int i = 0; i < s->len; ++i) nums[i] = 0;
  for (byte j = 0; j < s->len; ++j){
    nums[(s->data[j][col] | s->solution[j][col]) - 1]++;
  }
  // printf("Col Items : %i\n", col+1);
  for (int i = 0; i < s->len; ++i)
  {
    nums[i] = nums[i] ? nums[i] - 1 : 0;
  }
  for (int i = 0; i < s->len; ++i)
  {
    byte val = s->data[i][col];
    if(val) nums[val - 1] *= 100;
  }
  for (int i = 0; i < s->len; ++i)
  {
    // printf("%i\t", nums[i]);
    conflicts += nums[i];
  }
  // printf("\t TOTAL:%i \n", conflicts);
  return conflicts;
}
int evalSolution(Sudoku *s){
  int conflicts = 0;
  for (int i = 0; i < s->len; ++i)
  // for (int i = 0; i < 1; ++i)
  {
    conflicts += rowConflicts(s, i);
    conflicts += colConflicts(s, i);
  }
  s->err = conflicts;
  return conflicts;
}

/** Sudoku Constructive Solution Algorithm **/
byte* getSortedIndexes( byte* arr, byte len, SortType s){
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
    //note: 2/3 * 3 => 0, i/3 * 3 => 0,3,6
    byte n_row = (i/3)*3, n_col = (i*3)%9;
    for (byte j = 0; j < s->len; ++j)
    {
      if(s->data[j/3 + n_row][j%3 + n_col] != 0) n_sq++;
    }
    sqWeigth[i] = n_sq;
  }
  byte *indexes = getSortedIndexes(sqWeigth, s->len, MAXSORT);
  byte **sqOrder = (byte**)malloc(sizeof(byte) * 2);
  sqOrder[1] = sqWeigth;
  sqOrder[0] = indexes;
  return sqOrder;
}
byte* getNumsInSq(Sudoku *s, byte index, byte sz, byte* indexes){
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
  return cnt;
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
  return cnt;
}
// adds number to cell
byte addNumberSq(Sudoku *s, byte sqNum, byte idx, byte* nums, byte sz, byte w){
  byte n = 0;
  byte sqrow = (sqNum / 3) * 3;
  byte sqcol = (sqNum * 3) % 9;
  byte hardSol = w == 0;
  // byte hardSol = 0;
  
  for (int i = 0; i < sz; ++i)
  {
    if(nums[i] && (hardSol ||!(rand() % w--))){ // && only executes second part if the first passes
      s->solution[sqrow + (idx / 3)][sqcol + (idx % 3)] = nums[i];
      if(hardSol) {
        s->data[sqrow + (idx / 3)][sqcol + (idx % 3)] = nums[i];
        s->solution[sqrow + (idx / 3)][sqcol + (idx % 3)] = 0;
      }
      return i;
    }
  }
  printf("SHOULD NOT GET HERE!!!! \t SQUARE: %i \t INDEX: %i\n", sqNum+1, idx);
  return n;
}
void modifyWeights(byte **nums, byte *weigths, byte sz){
  byte* mw = (byte*)malloc(sizeof(byte) * sz);
  for (int k = 0; k < sz; ++k){ 
    mw[k] = 1;
    weigths[k] = 0;
  }
  for (int k = 0; k < sz; ++k)
  {
    for (int i = 0; i < sz; ++i)
    {
      if(nums[i][k] != 0)mw[k]++;
    }
    for (int i = 0; i < sz; ++i)
    {
      if(nums[i][k] != 0) weigths[i] += mw[k];
    }
  }
  free(mw);
}
void resetWeigths(byte **nums, byte *weigths, byte sz){
  for (int i = 0; i < sz; ++i)
  {
    weigths[i] = 0;
    for (int j = 0; j < sz; ++j)
    {
      if(nums[i][j] != 0) weigths[i]++;
    }
  }
}
void fillSqPrbs(Sudoku *s, byte index, byte sz){
  byte* indexes = (byte*)malloc(sizeof(byte) * sz);
  byte* weigths = (byte*)malloc(sizeof(byte) * sz);
  byte* notUsed = getNumsInSq(s, index, sz, indexes);
  byte **nums = allocByteArr(sz);
  byte sqrow = (index / 3) * 3;
  byte sqcol = (index * 3) % 9;
  for (int i = 0; i < sz; ++i){
    weigths[i] = sz;
    memcpy(nums[i], notUsed, sizeof(byte) * sz);
    weigths[i] -= getNumsInRow(s, sqrow + (indexes[i] / 3), nums[i], sz);
    weigths[i] -= getNumsInCol(s, sqcol + (indexes[i] % 3), nums[i], sz);
    // for (int k = 0; k < sz; ++k)
    // {
    //   printf("%i\t", nums[i][k]);
    // }
    // printf("\n");
    if(weigths[i] == 1){ //use as hard sol, should reduce problem size!
      addNumberSq(s, index, indexes[i], nums[i], sz, 0);
      free(notUsed);
      free(indexes);
      free(weigths);
      freeByteArr(nums);
      // if(index==1) printf("FOUND FIXED!!!\n");
      return fillSqPrbs(s, index, sz - 1);
    }
  }
  // printf("\n------------\n");
  modifyWeights(nums, weigths, sz);
  byte *order = getSortedIndexes(weigths, sz, MINSORT);

  for (int i = 0; i < sz; ++i)
  {
    resetWeigths(nums, weigths, sz);
    byte r_i = order[i]; //real index
    byte n = addNumberSq(s, index, indexes[r_i], nums[r_i], sz, weigths[r_i]);
    weigths[r_i] = 0;
    for (int j = 0; j < sz; ++j)
    {
      if(nums[j][n]) weigths[j] = weigths[j] ? weigths[j] - 1 : weigths[j];
      nums[j][n] = 0;
    }
    free(order);
    modifyWeights(nums, weigths, sz);
    order = getSortedIndexes(weigths, sz, MINSORT);
  }
  free(order);
  free(notUsed);
  free(indexes);
  free(weigths);
  freeByteArr(nums);
}

byte getNumFreeSpaces(Sudoku *s){
  byte cnt = 0;
  for (int i = 0; i < s->len; ++i)
  {
    for (int j = 0; j < s->len; ++j){
      cnt += (s->data[i][j] == 0 ? 1 : 0);
    }
  }
  return cnt;
}

byte constructiveSolution(Sudoku *s){
  byte freesp = getNumFreeSpaces(s);
  // printf("Number of Free Spaces\t%i\n", freesp);
  byte **sqOrder = getSqOrder(s);
  // should check free spaces before and after... if changed, re-do
  for (int i = 0; i < s->len; ++i)
  {
    byte index = sqOrder[0][i], weigth = sqOrder[1][index];
    // printf("SQUARE %i :\t", index+1);
    fillSqPrbs(s, index, s->len - weigth);
  }
  free(sqOrder[0]);
  free(sqOrder[1]);
  free(sqOrder);
  // if(freesp - getNumFreeSpaces(s)) return constructiveSolution(s);
  return getNumFreeSpaces(s);
}

byte evalOptSolution(Sudoku *s, SudokuSearchItem *opt){
  byte w = s->err;
  byte n_row = ((opt->square / 3) * 3 ) + (opt->from / 3),
      n_row2 = ((opt->square / 3) * 3 ) + (opt->to / 3);

  byte n_col = ((opt->square * 3) % 9 ) + (opt->from % 3),
      n_col2 = ((opt->square * 3) % 9 ) + (opt->to % 3);

  byte oConflicts = rowConflicts(s, n_row) + rowConflicts(s, n_row2)
                    + colConflicts(s, n_col) + colConflicts(s, n_col2);

  byte t = s->solution[n_row][n_col];
  s->solution[n_row][n_col] = s->solution[n_row2][n_col2];
  s->solution[n_row2][n_col2] = t;
  byte oConflicts2 = rowConflicts(s, n_row) + rowConflicts(s, n_row2)
                    + colConflicts(s, n_col) + colConflicts(s, n_col2);

  // printf("Solución cambio por %i conflictos\n*********************\n",
  //        (oConflicts2 - oConflicts));
  // printSudokuSol(s);
  //switch values

  //revert 
  t = s->solution[n_row][n_col];
  s->solution[n_row][n_col] = s->solution[n_row2][n_col2];
  s->solution[n_row2][n_col2] = t;
  return (byte)(w - (oConflicts - oConflicts2));
}
void testSolutions2Opt(Sudoku *s, int sq, int *count, SudokuSearchItem *opts){
  byte n_row = (sq / 3) * 3, n_col = (sq * 3) % 9;
  for (byte i = 0; i < s->len - 1; ++i)
  {
    byte n1 = s->solution[i/3 + n_row][i%3 + n_col];
    if(n1 != 0){
      for (byte j = i+1; j < s->len; ++j)
      {
        byte n2 = s->solution[j/3 + n_row][j%3 + n_col];
        if(n2 != 0 && n2 != n1){
          printf("Can switch %i with %i\n", n1, n2);
          opts[*count].square = sq;
          opts[*count].from = i;
          opts[*count].to = j;
          opts[*count].weigth = evalOptSolution(s, &opts[*count]);
          // return; //just to test one
          *count = *count + 1;
        }
      }
    }
  }
}
void applySolutions2Opt(Sudoku *s, SudokuSearchItem *opt){
  byte w = s->err;
  byte n_row = ((opt->square / 3) * 3 ) + (opt->from / 3),
      n_row2 = ((opt->square / 3) * 3 ) + (opt->to / 3);

  byte n_col = ((opt->square * 3) % 9 ) + (opt->from % 3),
      n_col2 = ((opt->square * 3) % 9 ) + (opt->to % 3);

  // printf("SQ: %i\tSe cambiara %i por %i\n", opt->square, s->solution[n_row][n_col], s->solution[n_row2][n_col2]);

  byte t = s->solution[n_row][n_col];
  s->solution[n_row][n_col] = s->solution[n_row2][n_col2];
  s->solution[n_row2][n_col2] = t;
}
byte* getSortedOpts( SudokuSearchItem* arr, byte len, SortType s){
  byte *indexes = (byte*)malloc(sizeof(byte) * len);
  for (byte i = 0; i < len; ++i) indexes[i] = i;
  for (byte i = 0; i < len-1; ++i)
  {
    for (byte j = len -1; j > i; j--)
    {
      if(
         ( s == MAXSORT && arr[indexes[i]].weigth < arr[indexes[j]].weigth) || 
         ( s == MINSORT && arr[indexes[i]].weigth > arr[indexes[j]].weigth) ||
         ( arr[indexes[i]].weigth == arr[indexes[j]].weigth && rand() % 2) //"coin toss"
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

/** Iterative Search **/
void localSearchSolution(Sudoku *s){
  #warning 324 value will only work for 9x9 sudoku...
  SudokuSearchItem *opts = (SudokuSearchItem*)malloc(sizeof(SudokuSearchItem) * 324);

  //do reallocs as needed....
  //do for every square, max new solutions per square(9 choose 2) => 36
  // total 324 all sudoku
  int z = 0;
  for (z = 0; z < 1; ++z)
  {
    int count = 0;
    for (int i = 0; i < s->len; ++i)
    {
      testSolutions2Opt(s, i, &count, opts);
    }
    byte *order = getSortedOpts(opts, count, MINSORT);
    for (int i = 0; i < count; ++i)
    {
      if(z >= 4)printf("PESO ITER %i : %i\n", z, opts[order[i]].weigth);
    }
    applySolutions2Opt(s, &opts[order[0]]);
    // printSudokuSol(s);
    // printf("ITER %i NUEVO PESO : %i\n\n", z, opts[order[0]].weigth);
    s->err = opts[order[0]].weigth;
    if(opts[order[0]].weigth == 0) break;
    free(order);
  }
  printf("ITERATIONS %i\n", z);
  free(opts);
}