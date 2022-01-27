#ifndef TEXT_H
#define TEXT_H

#include <string.h>
#include <stdio.h>
#include "num.h"

#define TEXT_MAXLEN 1000
typedef struct
{
	char str[TEXT_MAXLEN];
	char * end;
}text_t;

#define TEXT_INITIALIZER {.str[0] = '\0', .end = &text.str[0]}

text_t create_text();

#define print_in_text(text,format,...)\
	text.end += sprintf(text.end, format, __VA_ARGS__);

#define print_num_in_text(text,num)									\
	if(is_num_fraction(num))									\
        {												\
                print_in_text(text,"%d/%d",float_to_int(num.numerator),float_to_int(num.denominator));	\
        }												\
        else												\
        {												\
                if(is_num_int(num))									\
                {											\
                        print_in_text(text,"%d",float_to_int(num.numerator));				\
                }											\
                else											\
                {											\
                        print_in_text(text,"%.2f",get_num(num));					\
                }											\
        }

#define add_ch_in_text(text,ch)\
	*text.end = ch;        \
	text.end++;            \
	*text.end = '\0';

#define cat_str_in_text(text,str)	\
	strcat(text.end,str);		\
	while(*text.end != '\0')	\
	{				\
		text.end++;		\
	}

#define cat_text_in_text(dest,src)\
	cat_str_in_text(dest,src.str);

#define get_text_len(text)\
	(size_t)(text.end-text.str)

#endif
