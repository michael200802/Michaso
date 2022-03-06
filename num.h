#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

#ifndef NUM_H
#define NUM_H

typedef int64_t integer_t;
typedef uint64_t uinteger_t;

typedef struct
{
	integer_t numerator;
	integer_t denominator;
	enum num_state{NUM_STATE_FRACTION,NUM_STATE_DECIMAL,NUM_STATE_INT} state;
}num_t;

#define INITIALIZER_NUM {.numerator = 0, .denominator = 1}

#define set_num(num,_numerator,_denominator,_state)	\
	num.numerator = _numerator;						\
	num.denominator = _denominator;					\
	num.state = _state;

integer_t pow_integer(integer_t x, unsigned int y);

size_t get_length_integer(integer_t num);

#define abs_integer(num)\
	(num < 0 ? num*-1 : num)

bool is_num_greater(num_t num1, num_t num2);

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

