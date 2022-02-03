#include "text.h"

text_t create_text(const char * str)
{
	text_t text = {};

	allocate_mem(text,0);

	if(str != NULL)
	{
		for(;*str != '\0'; str++, text.len++)
		{
			text.str[text.len] = *str;
		}
	}

	return text;
}

short_text_t create_short_text(const char * str)
{
	short_text_t text = {};
	text.str = text._str;
	if(str != NULL)
	{
		for(;*str != '\0'; str++, text.len++)
		{
			text.str[text.len] = *str;
		}
	}

	return text;
}
