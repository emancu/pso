#include <lib_printf.h>

int printf_len(const char* fmt, va_list argp) {
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt);
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
          count += 1;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
          count += dec_into_string_len(x);
          break;
        case 'x':
          x = va_arg(argp, int);
          count += hex_into_string_len(x);
          break;
        case 's':
          str = va_arg(argp, char*);
          count += strlen(str);
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      i++;
      count++;
    }
  }
  va_end(argp);
  return count;
}


int sprintf_in(char* str, const char* fmt, va_list argp) {
  char ch;
  char* s;
  int x, i = 0, count = 0, len = strlen(fmt);
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
          if (char_into_string(str, &count, ch) < 0)
            return -1;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
          if (dec_into_string(str, &count, x) < 0)
            return -1;
          break;
        case 'x':
          x = va_arg(argp, int);
          if (hex_into_string(str, &count, x) < 0)
            return -1;
          break;

          break;
        case 's':
          s = va_arg(argp, char*);
          if (str_into_string(str, &count, s) < 0)
            return -1;
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      str[count] = fmt[i];
      i++;
      count++;
    }
  }
  str[count] = '\0';
  va_end(argp);
  return 0;
}


int sprintf(char* str, const char* fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  return sprintf_in(str, fmt, argp);
}
