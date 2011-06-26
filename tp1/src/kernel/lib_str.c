#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "lib_str.h"
#include "i386.h"

#define LARGEST_INT 0x7FFFFFFF

int strncpy(char* dest, const char* src, unsigned int count) {
	int i;
	for (i = 0; count == 0 || i < count; i++) {
		dest[i] = src[i];
		if (dest[i] == '\0')
			break;
	}
	return i;
}

int strcmp(const char* p, const char* q) {
	for (; *p && *q && *p == *q; p++, q++)
		;
	return *p ? (*q ? (int) *p - (int) *q : 1) : *q ? -1 : 0;
}

int strncmp(const char* p, const char* q, unsigned int n) {
	for (; n && *p && *q && *p == *q; p++, q++, n--)
		;
	return n ? (*p ? (*q ? (int) *p - (int) *q : 1) : *q ? -1 : 0) : 0;
}

int strchr(const char* str, int c) {
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == c)
			return i;
	}
	return -1;
}

int strlen(const char* str) {
	int len = 0;
	while (str[len] != '\0')
		len++;
	return len;
}

int dec_into_string_len(int num) {
	if (num < 0)
		return 1 + num_into_string_len((unsigned int) num, 10);
	else
		return num_into_string_len((unsigned int) num, 10);
}

int dec_into_string(char* str, int* index, int num) {
	unsigned int number = num;
	if (num < 0) {
		str[(*index)++] = '-';
		number = -num;
	}
	if (num_into_string(str, index, number, 10) < 0) {
		if (num < 0)
			(*index)--;
		return -1;
	}
	return 0;
}

int hex_into_string_len(int num) {
	return num_into_string_len((unsigned int) num, 16);
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

int str_into_string(char* str, int* index, const char* str2) {
	int len = strlen(str2), i = 0;
	while (i < len) {
		str[*index] = str2[i];
		i++;
		(*index)++;
	}
	return 0;
}

int mayuscula(int c) {
	int cM; /* Contiene el caracter convertido a mayuscula */
	/* Primer caso: Letras entre a y z */
	if (c >= 'a' && c <= 'z'){
		/* Se resta la diferencia entre minusculas y mayusculas en la tabla ASCII */
		cM = c - ('a' - 'A');
	}else{
		cM = c;
	}

	/* Devuelve el caracter convertido (o posiblemente intacto) */
	return (cM);
}

void str_convert_to_mayus(char* str, int from, int to) {
	while (from < to) {
		str[from] = mayuscula(str[from]);
		from++;
	}
}

int str_into_num(const char* str, unsigned int len, unsigned int base, unsigned int* num) {
	int count, i, digit, acum;
	char c;
	*num = 0;
	for (count = 0; len == 0 || count < len; count++) {
		c = str[count];
		//Si el caracter está entre 0 y 9 y debajo de la base
		if (c >= 0x30 && c < 0x3A && (c - 0x30) < base)
			continue;
		//Si el caracter está entre A y F y debajo de la base
		if (c >= 0x41 && c < 0x47 && (c + 10) < (base + 0x41))
			continue;
		//Si el caracter está entre a y f y debajo de la base
		if (c >= 0x61 && c < 0x67 && (c + 10) < (base + 0x61))
			continue;
		break;
	}
	if (count == 0)
		return -1;
	acum = 1;
	for (i = 0; i < count - 1; i++)
		acum *= base;

	for (i = 0; i < count; i++) {
		c = str[i];
		if (c >= 0x30 && c < 0x3A)
			digit = (c - 0x30);
		if (c >= 0x41 && c < 0x47)
			digit = (c - 0x41 + 10);
		if (c >= 0x61 && c < 0x67)
			digit = (c - 0x61 + 10);
		*num += digit * acum;
		acum /= base;
	}
	return count;
}

int str_into_dec(const char* str, unsigned int len, unsigned int* num) {
	int ret;
	if (str[0] == '-') {
		ret = str_into_num(str + 1, len, 10, num);
		*num = -(*num);
		return ret;
	} else {
		return str_into_num(str, len, 10, num);
	}
}

int str_into_hex(const char* str, unsigned int len, unsigned int* num) {
	if (str[0] == '0' && str[1] == 'x')
		return str_into_num(str + 2, len, 16, num);
	else
		return str_into_num(str, len, 16, num);
}

#endif
