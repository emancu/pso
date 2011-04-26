#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "lib_str.h"
#include "i386.h"

#define LARGEST_INT 0x7FFFFFFF

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    len++;
  return len;
}

int dec_into_string(char* str, int* index, int lim, int num) {
  unsigned int number = num;
  if (num < 0) {
    str[(*index)++] = '-';
    number = -num; 
  }
  if (num_into_string(str, index, lim, number, 10) < 0) {
    if (num < 0) (*index)--;
    return -1;
  }
  return 0; // Added this because we need to return something Always. Check it!
}

int hex_into_string(char* str, int* index, int lim, int num) {
  str[*index] = '0';
  (*index)++;
  str[*index] = 'x';
  (*index)++;
  if (num_into_string(str, index, lim, num, 16) < 0) {
    (*index) -= 2;
    return -1;
  }
  return 0;
}

int num_into_string(char* str, int* index, int lim, unsigned int num, int base) {
  int dignum = 1;
  int acum = base;
  unsigned int val;
  char* digits = "0123456789ABCDEF";
  while (num / acum >= base) {
    dignum += 1;
    acum *= base;
  }

  if (*index + dignum >= lim) return -1;

  while (acum > 0) {
  //  if (acum == 0) breakpoint();
    val = num / acum;
    str[*index] = digits[val];
    (*index)++;
    num -= val * acum;
  //  if (base == 0) breakpoint();
    acum /= base;
  }
  return 0;
}

int char_into_string(char* str, int* index, int lim, char c) {
  if (*index >= lim) return -1;
  str[*index] = c;
  (*index)++;
  return 0;
}

int str_into_string(char* str, int* index, int lim, char* str2) {
  int len = strlen(str2), i = 0;
  if (*index + len >= lim) return -1;
  while(i < len) {
    str[*index] = str2[i];
    i++;
    (*index)++;
  }
  return 0;
}

#endif
