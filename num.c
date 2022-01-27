#include "num.h"
#include <ctype.h>

num_t abs_num(num_t num)
{
        num.denominator = float_abs(num.denominator);
        num.numerator = float_abs(num.numerator);
        return num;
}

num_t simplify_num(num_t num)
{
        if(is_num_fraction(num) == false)
        {
                if(num.numerator == num.denominator)
                {
                        num.numerator = 1;
                        num.denominator = 1;
                }
                return num;
        }

        if(num.numerator < 0 && num.denominator < 0)
        {
                num.numerator *= -1;
                num.denominator *= -1;
        }

        for(int i = 2, max = float_abs(num.denominator) > float_abs(num.numerator) ? float_abs(num.denominator) : float_abs(num.numerator); i <= max; i++)
        {
                if(float_to_int(num.denominator)%i == 0 && float_to_int(num.numerator)%i == 0)
                {
                        num.numerator /= i;
                        num.denominator /= i;
                }
        }

        return num;
}


num_t sum_num(num_t num1, num_t num2)
{
        num1 = simplify_num(num1);
        num2 = simplify_num(num2);

        num1.numerator *= num2.denominator;
        num1.numerator += num2.numerator*num1.denominator;

        num1.denominator *= num2.denominator;

        num1 = simplify_num(num1);

        return num1;
}

num_t rest_num(num_t num1, num_t num2)
{
	num2.numerator *= -1;
	return sum_num(num1,num2);
}

num_t divide_num(num_t num1, num_t num2) 
{
        num1.numerator *= num2.denominator;
        num1.denominator *= num2.numerator;
        return num1;
}

num_t multiply_num(num_t num1, num_t num2)
{
	num1.numerator *= num2.numerator;
	num1.denominator *= num2.denominator;
	return num1;
}

bool str_to_num(const char * str, num_t * num)
{
	if(sscanf(str,"%f",&num->numerator) != 1)
	{
		return false;
	}

	if(*str == '-' || *str == '+')
	{
		str++;
	}
	if(num->numerator != (int)num->numerator)
	{
		while(*str != '.')
		{
			str++;
		}
		str++;
	}
	while(isdigit(*str))
	{
		str++;
	}

	if(*str == '/')
	{
		str++;
		if(sscanf(str,"%f",&num->denominator) != 1)
		{
			return false;
		}

		if(*str == '-' || *str == '+')
		{
			str++;
		}
		if(num->denominator != (int)num->denominator)
		{
			while(*str != '.')
			{
				str++;
			}
			str++;
		}
		while(isdigit(*str))
		{
			str++;
		}
	}
	else
	{
		num->denominator = 1;
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

void printnum(num_t num)
{
	printf("%f/%f ",num.numerator,num.denominator);
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
