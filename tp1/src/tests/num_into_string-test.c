#include <stdio.h>
#include "lib_str.h"

int main(void) {
	char buf[100];
	unsigned int index = 0, i = 0;
  hex_into_string(buf, &index, 100, 0xA);
  printf("Test: %s\n", buf); index = 0;
  hex_into_string(buf, &index, 100, 0xA0);
  printf("Test: %s\n", buf); index = 0;
  hex_into_string(buf, &index, 100, 0xA0F);
  printf("Test: %s\n", buf); index = 0;
  hex_into_string(buf, &index, 100, 0x1000000);
  printf("Test: %s\n", buf); index = 0;
  for (i = 0x10000000; i > 0; i++) {
		index = 0;
		if (hex_into_string(buf, &index, 100, i) < 0) {
			printf("Not enough space: %s\n", buf);
			return 0;
		}
		//printf("%s\n", buf);
		if (dec_into_string(buf, &index, 100, i) < 0) {
			printf("Not enough space: %s\n", buf);
			return 0;
		}
//		printf("%x :: %x\n", ((unsigned int)(0-1)), i);
	}
	printf("There was no error whatsoever\n");
	return 0;
}
