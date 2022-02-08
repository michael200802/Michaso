#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "num.h"

#ifndef TEXT_H
#define TEXT_H

#define TEXT_LEN 6000
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

#define is_text_short(text)\
	(sizeof(text._str) == sizeof(TEXT_MINLEN))

#define allocate_mem(text,nbytes)							\
	if(is_text_short(text) == false)						\
	{										\
		if(text.str == NULL)							\
		{									\
			text.allocated_bytes = TEXT_LEN*sizeof(char)+nbytes;		\
			text.str = (char*)malloc(text.allocated_bytes);			\
		}									\
		else if(text.allocated_bytes < (text.len+nbytes+1))			\
		{									\
			text.allocated_bytes += TEXT_LEN*sizeof(char)+nbytes;		\
			text.str = (char*)realloc(text.str,text.allocated_bytes);	\
		}									\
	}										\
	else										\
	{										\
		text.str = text._str;							\
	}

text_t create_text(const char * str);

short_text_t create_short_text(const char * str);

#define destroy_text(text)			\
	if(is_text_short(text) == false)\
	{								\
		free(text.str);				\
		text.str = NULL;			\
	}

#define clear_text(text)		\
	destroy_text(text);			\
	if(text.str != NULL)		\
	{							\
		text.str[0] = '\0';		\
	}							\
	text.len = 0;				\
	text.allocated_bytes = 0;

/*
#define text_to_short(text)\
	(*((short_text_t*)&(text)))
*/

#define get_text_end(text)\
	(text.str+text.len)

#define print_in_text(text,format,...)								\
	allocate_mem(text,200);											\
	text.len += sprintf(get_text_end(text), format, __VA_ARGS__);	\
	allocate_mem(text,0);

#define print_num_in_text(text,num)																	\
	if(is_num_fraction(num))																		\
	{																								\
			print_in_text(text,"%d/%d",float_to_int(num.numerator),float_to_int(num.denominator));	\
	}																								\
	else																							\
	{																								\
			if(is_num_int(num))																		\
			{																						\
					print_in_text(text,"%d",float_to_int(num.numerator));							\
			}																						\
			else																					\
			{																						\
					print_in_text(text,"%.2f",get_num(num));										\
			}																						\
	}

#define add_ch_in_text(text,ch)		\
	allocate_mem(text,1);			\
	*get_text_end(text) = ch;       \
	text.len++;            			\
	*get_text_end(text) = '\0';

#define cat_str_in_text(text,str)						\
	allocate_mem(text,strlen(str));						\
	strcpy(get_text_end(text),str);						\
	do													\
	{													\
		text.len++;										\
	}while(*get_text_end(text) != '\0');


#define cat_text_in_text(dest,src)				\
	allocate_mem(dest,src.len);					\
	strcpy(get_text_end(dest),src.str); 		\
	dest.len += src.len;

#define get_text_len(text)	\
	(text.len)

#endif
