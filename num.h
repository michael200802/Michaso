#ifndef NUM_H
#define NUM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>

typedef int64_t integer_t;
typedef uint64_t uinteger_t;

typedef struct
{
	uinteger_t numerator;
	uinteger_t denominator;
    uinteger_t int_part;
	enum num_state{NUM_STATE_FRACTION, NUM_STATE_DECIMAL, NUM_STATE_INT} state;
    enum num_sign{NUM_SIGN_POSITIVE, NUM_SIGN_NEGATIVE} sign;
}num_t;

typedef struct
{
	uinteger_t numerator;
	uinteger_t denominator;
}fraction_t;

typedef enum {NUM_GREATER,NUM_LESSER,NUM_EQUAL} compare_num_returnv_t;

compare_num_returnv_t compare_num(const num_t * const num1, const num_t * const num2);

compare_num_returnv_t compare_num_with_int(const num_t * const restrict num, integer_t integer);

void simplify_num(num_t * const restrict num);

void sum_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product);

void rest_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product);

void divide_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product);

void multiply_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product);

bool strtonum(const char * restrict str, num_t * const restrict num);

size_t printnum(char * restrict str, const num_t * const restrict num);

#define abs_integer(integer)\
	(integer < 0 ? integer*-1 : integer)

#define abs_num(num)\
	num.sign = NUM_SIGN_POSITIVE;

#define is_num_minus1(num)\
	(num.sign == NUM_SIGN_NEGATIVE && num.int_part == 1 && num.numerator == 0)

#define is_num_one(num)\
	(num.sign == NUM_SIGN_POSITIVE && num.int_part == 1 && num.numerator == 0)

#define is_num_zero(num)\
	(num.int_part == 0 && num.numerator == 0)

#define set_num(num,_numerator,_denominator,_sign,_state)	\
	num.int_part = 0;										\
	num.numerator = _numerator;						        \
	num.denominator = _denominator;					        \
    num.sign = _sign;                                       \
	num.state = _state;										\
	simplify_num(&num);

#define INITIALIZER_NUM (num_t){.sign = NUM_SIGN_POSITIVE, .state = NUM_STATE_FRACTION, .int_part = 0, .numerator = 0, .denominator = 1}

#endif