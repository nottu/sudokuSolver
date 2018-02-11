#include <stdlib.h>
#include <stdio.h>
#include "util.h"

/* Alloc square array*/
byte **allocByteArr(byte sz){
  byte **mtx = (byte**)malloc(sizeof(byte*) * sz);
  mtx[0] = (byte*)malloc(sizeof(byte) * sz * sz);
  for (byte i = 1; i < sz; ++i) {
    mtx[i] = mtx[i - 1] + sz;
  }
  return mtx;
}

void printByteArr(byte ** arr, byte sz){
  for (byte i = 0; i < sz; ++i)
  {
    for (byte j = 0; j < sz; ++j)
    {
      printf("%i ", arr[i][j]);
    }
    printf("\n");
  }
}

void freeByteArr(byte** arr){
  free(arr[0]);
  free(arr);
}