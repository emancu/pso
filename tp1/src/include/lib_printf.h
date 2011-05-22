#ifndef __LIB_PRINTF_H__
#define __LIB_PRINTF_H__

#include <tipos.h>
#include <stdarg.h>
#include <lib_str.h>

/* Esta función escribe en 'str' el contenido formateado de 'fmt' usando los demás parámetros. */
int sprintf(char* str, const char* fmt, ...);

/* Esta función escribe en 'str' el contenido formateado de 'fmt' usando los parametros en 'argp'
 * Completa al final con '\0' */
int sprintf_in(char* str, const char* fmt, va_list argp); 

/* Esta función calcula el largo del string resultado de formatear 'fmt' usando los valores en 'argp' 
 * El valor devuelto no considera un espacio más para ubicar \0 */
int printf_len(const char* fmt, va_list argp);

#endif
