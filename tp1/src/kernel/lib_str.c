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

int dec_into_string_len(int num) {
  if (num < 0)
    return 1 + num_into_string_len((unsigned int)num, 10);
  else
    return num_into_string_len((unsigned int)num, 10);
}

int dec_into_string(char* str, int* index, int num) {
  unsigned int number = num;
  if (num < 0) {
    str[(*index)++] = '-';
    number = -num; 
  }
  if (num_into_string(str, index, number, 10) < 0) {
    if (num < 0) (*index)--;
    return -1;
  }
  return 0;
}

int hex_into_string_len(int num) {
  return num_into_string_len((unsigned int)num, 16); 
}

int hex_into_string(char* str, int* index, int num) {
  // str[*index] = '0';
  // (*index)++;
  // str[*index] = 'x';
  // (*index)++;
  num_into_string(str, index, num, 16);
  return 0;
}

int num_into_string_len(unsigned int num, int base) {
  int dignum = 1;
  int acum = base;

  while (num / acum >= base) {
    dignum += 1;
    acum *= base;
  }

  return dignum;
}

int num_into_string(char* str, int* index, unsigned int num, int base) {
  int acum = 1;
  unsigned int val;
  char* digits = "0123456789ABCDEF";
  
  while (num / acum >= base) {
    acum *= base;
  }

  while (acum >= base) {
    val = num / acum;
    str[*index] = digits[val];
    (*index)++;
    num -= val * acum;
    acum /= base;
  }
  str[*index] = digits[num];
  (*index)++;
  return 0;
}

int char_into_string(char* str, int* index, char c) {
  str[*index] = c;
  (*index)++;
  return 0;
}

int str_into_string(char* str, int* index, char* str2) {
  int len = strlen(str2), i = 0;
  while(i < len) {
    str[*index] = str2[i];
    i++;
    (*index)++;
  }
  return 0;
}

#endif
