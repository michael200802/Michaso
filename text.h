#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "num.h"

#ifndef TEXT_H
#define TEXT_H

#define TEXT_LEN 1000
#define TEXT_MINLEN 50
typedef struct
{
	size_t allocated_bytes;
	size_t len;
	char * str;

	char _str[1];//it's useless
}text_t;

typedef struct
{
	size_t allocated_bytes;//it's useless
	size_t len;
	char * str;

	char _str[TEXT_MINLEN];
}short_text_t;

#include "resolve_matrix.h"

#define is_text_short(text) _Generic(text, short_text_t: true, text_t: false)\

#define allocate_mem(text,nbytes)																\
	if(is_text_short(text) == false)															\
	{																							\
		if(text.str == NULL	|| text.allocated_bytes < (text.len+nbytes+1))						\
		{																						\
			text.allocated_bytes += TEXT_LEN*sizeof(char)+nbytes;								\
			text.str = (char*)realloc(text.str,text.allocated_bytes);							\
		}																						\
	}																							\
	else																						\
	{																							\
		text.str = text._str;																	\
	}


text_t create_text(const char * str);

short_text_t create_short_text(const char * str);

#define destroy_text(text)								\
	if(is_text_short(text) == false && text.str != NULL)\
	{													\
		free(text.str);									\
		text.str = NULL;								\
	}

#define clear_text(text)		\
	destroy_text(text);			\
	if(text.str != NULL)		\
	{							\
		text.str[0] = '\0';		\
	}							\
	text.len = 0;				\
	text.allocated_bytes = 0;

#define get_text_end(text)\
	(text.str+text.len)

#define print_in_text(text,format,...)								\
	allocate_mem(text,200);											\
	text.len += sprintf(get_text_end(text), format, __VA_ARGS__);	\
	allocate_mem(text,0);

#define print_num_in_text(text,num)															\
	switch (num.state)																		\
	{																						\
		case NUM_STATE_INT:																	\
			print_in_text(text, "%"PRId64, num.numerator);									\
			break;																			\
		case NUM_STATE_FRACTION:															\
			print_in_text(text, "%"PRId64 "/" "%"PRId64, num.numerator, num.denominator);	\
			break;																			\
		case NUM_STATE_DECIMAL:																\
			print_in_text(text,"%.2lf",get_num(num));										\
			break;																			\
	}

#define add_ch_in_text(text,ch)	\
	allocate_mem(text,1);		\
	*get_text_end(text) = ch;   \
	text.len++;            		\
	*get_text_end(text) = '\0';

#define cat_str_in_text(text,str)			\
	allocate_mem(text, strlen(str));		\
	strcpy(get_text_end(text),str);			\
	while(*get_text_end(text) != '\0')		\
	{										\
		text.len++;							\
	}

#define cat_text_in_text(dest,src)				\
	allocate_mem(dest,src.len);					\
	strcpy(get_text_end(dest),src.str); 		\
	dest.len += src.len;

#define get_text_len(text)	\
	(text.len)

#endif
