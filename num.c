#include "num.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

inline integer_t pow_integer(integer_t x, unsigned int y)
{
	uinteger_t result = 1;
	bool is_negative = x < 0 ? true : false;

	if(is_negative)
	{
		x *= -1;
	}

	for(unsigned int i = 0; i < y; i++)
	{
		result *= x;
	}

	return ((integer_t)result) * (is_negative && y%2 == 0? -1 : 1);
}

inline size_t get_length_integer(integer_t num)
{
	size_t len = 0;
	do
	{
		num /= 10;
		len++;
	}while(num != 0);
	return len;
}

inline bool is_num_greater(num_t num1, num_t num2)
{
	num1 = simplify_num(num1);
	num2 = simplify_num(num2);

	num1.numerator *= num2.denominator;
	num2.numerator *= num1.denominator;
	/*
	num1.denominator *= num2.denominator;
	num2.denominator *= num1.denominator;
	*/

	if(num1.numerator > num2.numerator)
	{
		return true;
	}
	return false;
}

inline num_t abs_num(num_t num)
{
	num.denominator = abs_integer(num.denominator);
	num.numerator = abs_integer(num.numerator);
	return num;
}

inline num_t simplify_num(num_t num)
{
	bool is_negative;

	if(num.numerator == 0)
	{
		num.denominator = 1;
	}
	else
	{
		if(num.denominator < 0)
		{
			num.denominator *= -1;
			num.numerator *= -1;
		}

		is_negative = num.numerator < 0 ? true : false;
		if(is_negative)
		{
			num.numerator = abs_integer(num.numerator);
		}

		if(num.denominator == num.numerator)
		{
			num.numerator = 1;
			num.denominator = 1;
		}
	}

	if(num.denominator == 1)
	{
		if(is_negative)
		{
			num.numerator *= -1;
		}
		if(num.state == NUM_STATE_FRACTION)
		{
			num.state = NUM_STATE_INT;
		}
		return num;
	}

	/*
	uinteger_t max = num.denominator < num.numerator ? num.denominator :  num.numerator;
	for(uinteger_t i = 2; i <= max; i++)
	{
		if(num.denominator%i == 0 && num.numerator%i == 0)
		{
			num.numerator /= i;
			num.denominator /= i;

			i = 1;
			max = num.denominator < num.numerator ? num.denominator :  num.numerator;
		}
	}
	*/

	uinteger_t dividen, divisor, remainder, aux;
	do
	{
		for(dividen = num.numerator, divisor = num.denominator; divisor != 0;)
		{
			remainder = dividen%divisor;
			dividen = divisor;
			divisor = remainder; 
		}
		num.numerator /= dividen;
		num.denominator /= dividen;
	} while (dividen != 1);

	if(is_negative)
	{
		num.numerator *= -1;
	}

	if(num.state != NUM_STATE_DECIMAL)
	{
		if(num.denominator == 1)
		{
			num.state = NUM_STATE_INT;
		}
		else
		{
			num.state = NUM_STATE_FRACTION;
		}
	}

    return num;
}

inline num_t sum_num(num_t num1, num_t num2)
{
	num1 = simplify_num(num1);
	num2 = simplify_num(num2);

	num1.numerator *= num2.denominator;
	num1.numerator += num2.numerator*num1.denominator;

	num1.denominator *= num2.denominator;

	num1 = simplify_num(num1);

	if(num2.state == NUM_STATE_DECIMAL)
	{
		num1.state = NUM_STATE_DECIMAL;
	}

	return num1;
}

inline num_t rest_num(num_t num1, num_t num2)
{
	num2.numerator *= -1;
	return sum_num(num1,num2);
}

inline num_t divide_num(num_t num1, num_t num2)
{
	num_t aux;

	num1 = simplify_num(num1);
	num2 = simplify_num(num2);

	aux.numerator = num1.numerator;
	aux.denominator = num2.denominator;
	aux = simplify_num(aux);
	num1.numerator = aux.numerator;
	num2.denominator = aux.denominator;

	aux.numerator = num2.numerator;
	aux.denominator = num1.denominator;
	aux = simplify_num(aux);
	num2.numerator = aux.numerator;
	num1.denominator = aux.denominator;

	num1.numerator *= num2.denominator;
	num1.denominator *= num2.numerator;

	num1 = simplify_num(num1);

	if(num2.state == NUM_STATE_DECIMAL)
	{
		num1.state = NUM_STATE_DECIMAL;
	}

	return num1;
}

inline num_t multiply_num(num_t num1, num_t num2)
{
    num1 = simplify_num(num1);
    num2 = simplify_num(num2);

	num1.numerator *= num2.numerator;
	num1.denominator *= num2.denominator;

    num1 = simplify_num(num1);

	if(num2.state == NUM_STATE_DECIMAL)
	{
		num1.state = NUM_STATE_DECIMAL;
	}

	return num1;
}

num_t gcd_num(size_t nnums, ...)
{
	num_t * nums = (num_t*)calloc(nnums,sizeof(num_t));
	num_t gdc;
	va_list va;

	va_start(va,nnums);

	for(size_t i = 0; i < nnums; i++)
	{
		nums[i] = va_arg(va,num_t);
	}

	//iguala a todos bajo el mismo denominador
	for(size_t i = 0; i < nnums; i++)
	{
		for (size_t j = 0; j < nnums; j++)
		{
			if(nums[j].denominator != nums[i].denominator)
			{
				num_t aux = nums[j];

				nums[j].denominator *= nums[i].denominator;
				nums[j].numerator *= nums[i].denominator;

				nums[i].denominator *= aux.denominator;
				nums[i].numerator *= aux.denominator;
			}
		}
		for (size_t j = nnums-1;; j--)
		{
			if(nums[j].denominator != nums[i].denominator)
			{
				num_t aux = nums[j];

				nums[j].denominator *= nums[i].denominator;
				nums[j].numerator *= nums[i].denominator;

				nums[i].denominator *= aux.denominator;
				nums[i].numerator *= aux.denominator;
			}
			if(j == 0)
			{
				break;
			}
		}
	}

	for(size_t i = 0; i < nnums; i++)
	{
		printnum(nums[i]);
	}
	putchar(' ');

	free(nums);

	va_end(va);

	return gdc;
}

bool str_to_num(const char * str, num_t * num)
{
	integer_t pow10 = 1;

	if(sscanf(str,"%"SCNd64,&num->numerator) != 1)
	{
		return false;
	}

	if(*str == '-' || *str == '+')
	{
		str++;
	}

	while(isdigit(*str))
	{
		str++;
	}
	if(*str == '.')
	{
		integer_t aux;

		str++;
		while(*str == '0')
		{
			*str++;
			pow10*=10;
		}
		if(sscanf(str,"%"SCNd64,&aux) != 1)
		{
			return false;
		}

		pow10 *= pow_integer(10,get_length_integer(aux));

		num->numerator *= pow10;
		num->numerator += aux;

		num->state = NUM_STATE_DECIMAL;
	}
	while(isdigit(*str))
	{
		str++;
	}

	while(isspace(*str))
	{
		str++;
	}

	if(*str == '/')
	{
		num->state = NUM_STATE_FRACTION;

		str++;
		if(sscanf(str,"%"SCNd64,&num->denominator) != 1)
		{
			return false;
		}
		num->denominator *= pow10;

		if(*str == '-' || *str == '+')
		{
			str++;
		}

		while(isdigit(*str))
		{
			str++;
		}
		if(*str == '.')
		{
			integer_t aux;

			str++;
			pow10 = 1;
			while(*str == '0')
			{
				pow10*=10;
				str++;
			}
			if(sscanf(str,"%"SCNd64,&aux) != 1)
			{
				return false;
			}

			pow10 *= pow_integer(10,get_length_integer(aux));

			num->denominator *= pow10;
			num->denominator += aux;

			num->numerator *= pow10;
		}
		while(isdigit(*str))
		{
			str++;
		}
	}
	else
	{
		num->denominator = pow10;
		if(pow10 == 1)
		{
			num->state = NUM_STATE_INT;
		}
	}

	while(isspace(*str))
	{
		str++;
	}
	if(*str != '\0')
	{
		return false;
	}

	return true;
}

inline void printnum(num_t num)
{
	switch (num.state)
	{
		case NUM_STATE_INT:
			printf("%"PRId64, num.numerator);
			break;
		case NUM_STATE_FRACTION:
			printf("%"PRId64 "/" "%"PRId64, num.numerator, num.denominator);
			break;
		case NUM_STATE_DECIMAL:
			{
				integer_t modulus, digit1, digit2;

				modulus = num.numerator % num.denominator;
				digit1 = (modulus*10)/num.denominator;

				modulus =  (modulus*10)%num.denominator;
				digit2 = (modulus*10)/num.denominator;

				printf("%"PRId64".%"PRId64, num.numerator / num.denominator, digit1*10 + digit2);
			}
			break;
	}
}
