#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "klib_print.h"

//#include <stdio.h>
#include <stdarg.h>

static char line = 0;

char make_format(char blink, char front, char back, char bright) {
  char fmt = (char)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
  fmt |= ((back & 0x7) << 4);
  fmt |= ((blink > 0) << 7);
  return fmt;
}

void fill_screen(char color, char bright) {
  char* video = (char*)VIDEO_ADDR;
  char fmt = make_format(0x0, 0x0, color, bright);
  while((int)video < VIDEO_ADDR + SCR_W*SCR_H) {
    *video++ = 0x00;
    *video++ = fmt;
  }
}

void clear_screen() {
  fill_screen(0,0);
}

void move_scr_up() {
  char* video = (char*)VIDEO_ADDR;
  while ((int)video < (VIDEO_ADDR + (SCR_H-1)*(SCR_W))) {
    *video = *(video+SCR_W);
    video++;
  }
  char bkg = *(video-1);
  while ((int)video < VIDEO_ADDR + SCR_W * SCR_H) {
    *video = 0x00;
    *(video+1) = bkg;
    video+=2;
  }
}

int printk_resolver(int amount, char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, c = 0, len = strlen(fmt), arg = 0;
//  va_list argp;
//  va_start(argp, fmt);
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
          if (char_into_string(buff, &c, amount, ch) < 0)
            return -1;
          arg++;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
          if (dec_into_string(buff, &c, amount, x) < 0)
            return -1;
          arg++;
          break;
        case 'x':
          x = va_arg(argp, int);
          if (hex_into_string(buff, &c, amount, x) < 0)
            return -1;
          arg++;
          break;

          break;
        case 's':
          str = va_arg(argp, char*);
          if (str_into_string(buff, &c, amount, str) < 0)
            return -1;
          arg++;
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      if (c >= amount) return -1;
      buff[c] = fmt[i];
      i++;
      c++;
    }
  }
  buff[c] = '\0';
  printk_raw(buff, 0, 0xF, 0x0, 1);
  va_end(argp);
  return 0;
}

void printk(char* fmt, ...) {
  va_list argp;
  int amount = 8;
  va_start(argp, fmt);
  while(printk_resolver(amount, fmt, argp) < 0) {
    va_start(argp, fmt);
    amount *= 2;
  }
}

void printk_raw(char* str, char blink, char front, char back, char bright) {
  char* video = (char*)(VIDEO_ADDR + SCR_W * line);
  char fmt = make_format(blink, front, back, bright);
  while(*str != '\0') {
    if (*str == '\n') {
      if (line < SCR_H-1) {
        video = (char*)(((int)video) + (SCR_W - ((int)video - VIDEO_ADDR)));
        line++;
      } else {
        move_scr_up();
        video = (char*)(VIDEO_ADDR + (SCR_H-1)*SCR_W);
      }
    } else {
      *(video) = *str;
      *(video+1) = fmt;
      video += 2;
    }
    str++;
  }
}

int strlen(char* str) {
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
  int dignum = 1, acum = base, val;
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
