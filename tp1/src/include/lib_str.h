#ifndef __KLIB_PRINT_H__
#define __KLIB_PRINT_H__

//in [0-9a-fA-f]
#define isDigit(x) ((0x30 <= (char)(x) && (char)(x) <= 0x39) || (0x41 <= (char)(x) && (char)(x) <= 0x46) || (0x61 <= (char)(x) && (char)(x) <= 0x67))
//in [a-zA-z]
#define isCharacter(x) ((0x41 <= (char)(x) && (char)(x) <= 0x5A) || (0x61 <= (char)(x) && (char)(x) <= 0x7A))

#define isSymbol(x) ((0x21 <= (char)(x) && (char)(x) <= 0x2F) || (0x3A <= (char)(x) && (char)(x) <= 0x40) || (0x5B <= (char)(x) && (char)(x) <= 0x60) || (0x7B <= (char)(x) && (char)(x) <= 0x7E))

#define isSpace(x) (((char)(x) == 0x20) || ((char)(x) == '\n'))

/* Copies up to 'count' chars from 'src' to 'dest'.
 * If 'count' == 0 then copies until it finds a '\0'
 * in 'src'. If 'src' has a '\0' character before
 * 'count' chars are copied the function also stops.
 * It returns the number of chars copied. */
int strncpy(char* dest, const char* src, unsigned int count);

int strcmp(const char* p, const char* q);
int strncmp(const char* p, const char* q, unsigned int n);

/* Finds the first appearence of int 'c' in 'str'.
 * Searches from 'str' to strlen(str). 
 * Returns the index where 'c' was found, or 
 * -1 if it was not present. */
int strchr(const char* str, int c);

/* Converts 'num' to decimal string of numbers in 'str' , 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * maintained in its original value. */
int dec_into_string(char* str, int* index, int num);

/* Converts 'num' to hexadecimal string of numbers in 'str' , 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * was successful, -1 otherwise. If conversion was not successful 'index' is
 * maintained in its original value. */
int hex_into_string(char* str, int* index, int num);

/* Converts 'num' to string in 'str' using base 'base', 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * was successful, -1 otherwise. If conversion was not successful 'index' is
 * maintained in its original value. 
 * 'base' must be a number between 2 and 16 (both included) */
int num_into_string(char* str, int* index, unsigned int num, int base);

/* Inserts the given character in 'c' into 'str'. It first checks 
 * 'index' is set previous to 'lim'. Returns 0 if successful, moving index; -1 otherwise.
 * If unsuccessful *index is not moved. */
int char_into_string(char* str, int* index, char c);

/* Inserts given string in 'str2' into 'str'. Function first checks
 * that doing so will not exceed 'lim'. If so, function returns -1.
 * Otherwise it copies 'str2' into 'str' moving 'index'. 
 * Function expects a null terminated string in 'str2'.*/
int str_into_string(char* str, int* index, char* str2);

/* Returns the length of 'str'. Lenght is the number of characters
 * from str[0] until the first '\0'. */
int strlen(const char* str);

/* Calculates how many digits are necessary to write 'num' in base 'base' */
int num_into_string_len(unsigned int num, int base);

/* Calculates how many digits are necessary to write 'num' in base 10
 * It adds one to the count if num is negative considering minus sign. */
int dec_into_string_len(int num);

/* Calculates how many digits are necessary to write 'num' in base 16. */
int hex_into_string_len(int num);

/* Obtains a number from the following string, searching up to 'len'
 * characters ('len' == 0 uses as much of the string as possible).
 * The number is returned in 'num'. The result is > 0
 * if the string was parsed, < 0 if not. If part of the string was
 * parsed, the return value is the number of characters used.
 * The function goes through the string, expecting all characters 
 * which number is one between 0 and base -1. 
 * If something different is encountered, the
 * function will return -1 as an error (-1 only happens when the
 * string did not start with a valid digit). */
int str_into_num(const char* str, unsigned int len, unsigned int base, unsigned int* num);

/* Obtains a decimal number from string 'str', searching up to 'len'
 * characters ('len' == 0 uses as much of the string as possible). 
 * -1 is return in case of error (no valid number was obtainable). 
 * Parsed number is returned in 'num'. 
 * Return value is the number of characters used. */
int str_into_dec(const char* str, unsigned int len, unsigned int* num);

/* Obtains a hexadecimal number from string 'str', searching up to 'len'
 * characters ('len' == 0 uses as much of the string as possible). 
 * -1 is return in case of error (no valid number was obtainable). 
 * Parsed number is returned in 'num'. Return value is the number of characters used. */
int str_into_hex(const char* str, unsigned int len, unsigned int* num);
#endif
