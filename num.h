#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

#ifndef NUM_H
#define NUM_H

typedef struct
{
	int64_t numerator;
	int64_t denominator;
	enum num_state{NUM_STATE_FRACTION,NUM_STATE_DECIMAL,NUM_STATE_INT} state;
}num_t;

#define INITIALIZER_NUM {.numerator = 0, .denominator = 1}

#define set_num(num,_numerator,_denominator,_state)\
	num.numerator = _numerator;									\
	num.denominator = _denominator;								\
	num.state = _state;

#define get_num(num)\
	( num.state == NUM_STATE_INT ? num.numerator : ((double)num.numerator/(double)num.denominator) )

int64_t pow_int64(int64_t x, unsigned int y);

size_t get_length_int64(int64_t num);

#define abs_int64(num)\
	(num < 0 ? num*-1 : num)

bool is_num_greater(num_t num1, num_t num2);

num_t float_to_num(double _num, char precision);

num_t abs_num(num_t num);

num_t simplify_num(num_t num);

num_t sum_num(num_t num1, num_t num2);

num_t rest_num(num_t num1, num_t num2);

num_t divide_num(num_t num1, num_t num2);

num_t multiply_num(num_t num1, num_t num2);

num_t gcd_num(size_t nnums, ...);

bool str_to_num(const char * str, num_t * num);

void printnum(num_t num);

#endif

