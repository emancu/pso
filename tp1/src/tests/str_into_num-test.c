#include <stdio.h>
#include <lib_str.h>

int main(void) {
  char str1[] = "5";
  char str2[] = "A";
  char str3[] = "123456";
  char str4[] = "ABC123";
  char str5[] = "-123";
  char str6[] = "0xABC";

  int num;
  str_into_num(str1, 0, 10, &num); 
  printf("%s = %d\n", str1, num);
  str_into_num(str2, 0, 10, &num); 
  printf("%s = %d\n", str2, num);
  str_into_num(str2, 0, 16, &num); 
  printf("%s = %d\n", str2, num);
  str_into_num(str3, 0, 10, &num); 
  printf("%s = %d\n", str3, num);
  str_into_num(str3, 0, 16, &num); 
  printf("%s = %d\n", str3, num);
  str_into_num(str4, 0, 10, &num); 
  printf("%s = %d\n", str4, num);
  str_into_num(str4, 0, 16, &num); 
  printf("%s = %d\n", str4, num);
  str_into_num(str4, 3, 16, &num); 
  printf("%s = %d\n", str4, num);
  str_into_num(str4, 2, 16, &num); 
  printf("%s = %d\n", str4, num);
  str_into_dec(str1, 0, &num); 
  printf("%s = %d\n", str1, num);
  str_into_dec(str2, 0, &num); 
  printf("%s = %d\n", str2, num);
  str_into_hex(str2, 0, &num); 
  printf("%s = %d\n", str2, num);
  str_into_dec(str3, 0, &num); 
  printf("%s = %d\n", str3, num);
  str_into_hex(str3, 0, &num); 
  printf("%s = %d\n", str3, num);
  str_into_dec(str4, 0, &num); 
  printf("%s = %d\n", str4, num);
  str_into_hex(str4, 0, &num); 
  printf("%s = %d\n", str4, num);
  str_into_hex(str4, 3, &num); 
  printf("%s = %d\n", str4, num);
  str_into_hex(str4, 2, &num); 
  printf("%s = %d\n", str4, num);
  str_into_dec(str5, 0, &num); 
  printf("%s = %d\n", str5, num);
  str_into_hex(str6, 0, &num); 
  printf("%s = %d\n", str6, num);
  return 0;
}
