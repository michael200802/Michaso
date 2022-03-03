#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "num.h"

#ifndef TEXT_H
#define TEXT_H

#define TEXT_MAXLEN 3000
#define TEXT_LEN 1000
#define TEXT_MINLEN 50

typedef struct
{
	size_t len;
	char str[TEXT_MAXLEN];
}long_text_t;

typedef struct
{
	size_t len;
	char str[TEXT_LEN];
}text_t;

typedef struct
{
	size_t len;
	char str[TEXT_MINLEN];
}short_text_t;

text_t create_text(const char * str);

short_text_t create_short_text(const char * str);

#define init_text(text)			\
	text.str[0] = '\0';			\
	text.len = 0;

#define get_text_end(text)\
	(text.str+text.len)

#define print_in_text(text,format,...)								\
	text.len += sprintf(get_text_end(text), format, __VA_ARGS__);

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
	*get_text_end(text) = ch;   \
	text.len++;            		\
	*get_text_end(text) = '\0';

#define add_nch_in_text(text,_n,ch)		\
	{									\
		size_t i, n = _n;				\
		for(i = 0; i < n; i++)			\
		{								\
			*get_text_end(text) = ch;	\
			text.len++;					\
		}								\
		*get_text_end(text) = '\0';		\
	}

#define cat_str_in_text(text,_str)				\
	{											\
		const char * str;						\
		for(str = _str; *str != '\0'; str++)	\
		{										\
			*get_text_end(text) = *str;			\
			text.len++;							\
		}										\
		*get_text_end(text) = '\0';				\
	}

#define cat_text_in_text(dest,src)				\
	cat_str_in_text(dest, src.str);

#define get_text_len(text)	\
	(text.len)

#endif
