#include "num.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

inline int64_t pow_int64(int64_t x, unsigned int y)
{
	uint64_t result = 1;
	bool is_negative = x < 0 ? true : false;

	if(is_negative)
	{
		x *= -1;
	}

	for(unsigned int i = 0; i < y; i++)
	{
		result *= x;
	}

	return ((int64_t)result) * (is_negative && y%2 == 0? -1 : 1);
}

inline size_t get_length_int64(int64_t num)
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
	num.denominator = abs_int64(num.denominator);
	num.numerator = abs_int64(num.numerator);
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
			num.numerator = abs_int64(num.numerator);
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
	
	uint64_t max = num.denominator < num.numerator ? num.denominator :  num.numerator;
	for(uint64_t i = 2; i <= max; i++)
	{
		if(num.denominator%i == 0 && num.numerator%i == 0)
		{
			num.numerator /= i;
			num.denominator /= i;

			i = 1;
			max = num.denominator < num.numerator ? num.denominator :  num.numerator;
		}
	}

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

	return num1;
}

inline num_t rest_num(num_t num1, num_t num2)
{
	num2.numerator *= -1;
	return sum_num(num1,num2);
}

inline num_t divide_num(num_t num1, num_t num2)
{
	num1 = simplify_num(num1);
	num2 = simplify_num(num2);

	num1.numerator *= num2.denominator;
	num1.denominator *= num2.numerator;

	num1 = simplify_num(num1);

	return num1;
}

inline num_t multiply_num(num_t num1, num_t num2)
{
    num1 = simplify_num(num1);
    num2 = simplify_num(num2);

	num1.numerator *= num2.numerator;
	num1.denominator *= num2.denominator;

    num1 = simplify_num(num1);

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
	int64_t pow10 = 1;

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
		int64_t aux;

		str++;
		if(sscanf(str,"%"SCNd64,&aux) != 1)
		{
			return false;
		}

		pow10 = pow_int64(10,get_length_int64(aux));

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
			int64_t aux;

			str++;
			if(sscanf(str,"%"SCNd64,&aux) != 1)
			{
				return false;
			}

			pow10 = pow_int64(10,get_length_int64(aux));

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
	putchar(' ');

	switch (num.state)
	{
		case NUM_STATE_INT:
			printf("%"PRId64, num.numerator);
			break;
		case NUM_STATE_FRACTION:
			printf("%"PRId64 "/" "%"PRId64, num.numerator, num.denominator);
			break;
		case NUM_STATE_DECIMAL:
			printf("%.2lf",get_num(num));
			break;
	}

	//printf("%.2f/%.2f",num.numerator,num.denominator);
	putchar(' ');
}

/*
#include <stdlib.h>
int main(int argc, char ** argv)
{
	argv++;
	if(*argv == NULL)
	{
		return -1;
	}
	puts("...");

	size_t nnums = argc-1;
	num_t * array = (num_t*)calloc(nnums,sizeof(num_t));

	for(size_t i = 0; i < nnums; i++)
	{
		if(str_to_num(argv[i],array+i) == false)
		{
			puts("...");
			return -1;
		}
		printnum(array[i]);
	}

	num_t result = INITIALIZER_NUM;

	//sacar el total de la suma
	for(size_t i = 0; i < nnums; i++)
	{
		result = sum_num(result,array[i]);
	}
	puts("Suma");
	printnum(result);

	//sacar el total de la resta
	result = rest_num(result,result);
	for(size_t i = 1; i < nnums; i++)
	{
		result = rest_num(result,array[i]);
		result = abs_num(result);
	}
	puts("Resta");
	printnum(result);

	puts("...");
	return 0;
}
*/
