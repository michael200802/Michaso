#include <stdio.h>
#include <stdbool.h>

#ifndef NUM_H
#define NUM_H

typedef struct
{
	float numerator;
	float denominator;
}num_t;

#define INITIALIZER_NUM {.numerator = 0, .denominator = 1}

#define set_num(num,_numerator,_denominator)	\
	num.numerator = _numerator;		\
	num.denominator = _denominator;

#define float_to_int(num)\
	((int)num)

#define is_float_int(num)\
	num == (int)num

#define is_num_fraction(num)\
	is_float_int(num.numerator) && is_float_int(num.denominator) && num.denominator != 1

#define is_num_int(num)\
	num.denominator == 1 && is_float_int(num.numerator)

#define float_abs(num)\
	num < 0 ? num*-1 : num

#define get_num(num)\
	num.numerator/num.denominator

num_t abs_num(num_t num);

num_t simplify_num(num_t num);

num_t sum_num(num_t num1, num_t num2);

num_t rest_num(num_t num1, num_t num2);

num_t divide_num(num_t num1, num_t num2);

num_t multiply_num(num_t num1, num_t num2);

bool str_to_num(const char * str, num_t * num);

void printnum(num_t num);

#endif

