#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "lib_str.h"

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    len++;
  return len;
}

int dec_into_string(char* str, int* index, int lim, int num) {
  return num_into_string(str, index, lim, num, 10);
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

int num_into_string(char* str, int* index, int lim, int num, int base) {
  int dignum = 1, val;
  long long int acum = base;
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    dignum += 1;
    acum *= base;
  }
  acum /= base;

  if (num < 0) dignum++;
  if (*index + dignum >= lim) return -1;

  if (num < 0) {
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    val = num / acum;
    str[*index] = digits[val];
    (*index)++;
    num -= val * acum;
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
