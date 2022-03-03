#include "text.h"

text_t create_text(const char * str)
{
	text_t text;
	init_text(text);

	for(;*str != '\0'; str++, text.len++)
	{
		text.str[text.len] = *str;
	}

	return text;
}

short_text_t create_short_text(const char * str)
{
	short_text_t text;
	init_text(text);
	
	for(;*str != '\0'; str++, text.len++)
	{
		text.str[text.len] = *str;
	}

	return text;
}
