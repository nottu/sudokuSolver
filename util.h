#ifndef UTIL_H
#define UTIL_H
typedef unsigned short byte;
typedef enum sortType { MINSORT, MAXSORT} SortType;

byte** allocByteArr(byte sz);
void printByteArr(byte ** arr, byte sz);
void freeByteArr(byte** arr);
#endif