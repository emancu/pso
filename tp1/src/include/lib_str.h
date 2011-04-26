#ifndef __KLIB_PRINT_H__
#define __KLIB_PRINT_H__

/* Converts 'num' to decimal string of numbers in 'str' , 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * was successful, -1 otherwise. If conversion was not successful 'index' is
 * maintained in its original value. */
int dec_into_string(char* str, int* index, int lim, int num);

/* Converts 'num' to hexadecimal string of numbers in 'str' , 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * was successful, -1 otherwise. If conversion was not successful 'index' is
 * maintained in its original value. */
int hex_into_string(char* str, int* index, int lim, int num);

/* Converts 'num' to string in 'str' using base 'base', 
 * 'str' is filled starting in 'index' and moving it forward.
 * It does not move 'index' beyond 'lim'. The function returns 0 if the conversion
 * was successful, -1 otherwise. If conversion was not successful 'index' is
 * maintained in its original value. 
 * 'base' must be a number between 2 and 16 (both included) */
int num_into_string(char* str, int* index, int lim, unsigned int num, int base);

/* Inserts the given character in 'c' into 'str'. It first checks 
 * 'index' is set previous to 'lim'. Returns 0 if successful, moving index; -1 otherwise.
 * If unsuccessful *index is not moved. */
int char_into_string(char* str, int* index, int lim, char c);

/* Inserts given string in 'str2' into 'str'. Function first checks
 * that doing so will not exceed 'lim'. If so, function returns -1.
 * Otherwise it copies 'str2' into 'str' moving 'index'. 
 * Function expects a null terminated string in 'str2'.*/
int str_into_string(char* str, int* index, int lim, char* str2);

/* Returns the length of 'str'. Lenght is the number of characters
 * from str[0] until the first '\0'. */
int strlen(const char* str);

#endif
