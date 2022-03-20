#include "num.h"

inline compare_num_returnv_t compare_num(const num_t * const num1, const num_t * const num2)
{
    //- + which sign?
    if(num1->sign != num2->sign)
    {
        if(num2->sign == NUM_SIGN_NEGATIVE)
        {
            return NUM_GREATER;
        }
        return NUM_LESSER;
    }

    //compare int parts 
    if(num1->int_part > num2->int_part)
    {
        return NUM_GREATER;
    }
    else if(num1->int_part < num2->int_part)
    {
        return NUM_LESSER;
    }

    //it's a homogeneous fraction
    if(num1->denominator == num2->denominator)
    {
        if(num1->numerator == num2->numerator)
        {
            return NUM_EQUAL;
        }
        return (num1->numerator > num2->numerator ? NUM_GREATER : NUM_LESSER);
    }

    /*
    //difference between denominator and numerator (denominator - numerator)
    uinteger_t dif_num1 = num1->denominator - num1->numerator;
    uinteger_t dif_num2 = num2->denominator - num2->numerator;

    //how away are they from becoming 1?
    if(dif_num1 == dif_num2)
    {
        return (num1->denominator > num2->denominator ? NUM_GREATER : NUM_LESSER);
    }
    if(num1->denominator > num2->denominator)
    {
        if(dif_num1 < dif_num2)
        {
            return NUM_GREATER;
        }
        //...
    }
    if(dif_num1 > dif_num2)
    {
        return NUM_LESSER;
    }
    //..
    */

   if(num1->numerator == 0)
   {
       return NUM_LESSER;
   }
   else if(num2->numerator == 0)
   {
       return NUM_GREATER;
   }

    uinteger_t quotient1 = num1->denominator / num1->numerator, remainder1 = num1->denominator % num1->numerator;
    uinteger_t quotient2 = num2->denominator / num2->numerator, remainder2 = num2->denominator % num2->numerator;

    if(quotient1 == quotient2)
    {
        if(remainder1 == remainder2)
        {
            if(num1->denominator > num2->denominator)
            {
                return NUM_GREATER;
            }
            return NUM_LESSER;
        }
        if(remainder1 < remainder2)
        {
            return NUM_GREATER;
        }
        return NUM_LESSER;
    }
    if(quotient1 < quotient2)
    {
        return NUM_GREATER;
    }
    return NUM_LESSER;
}

inline compare_num_returnv_t compare_num_with_int(const num_t * const restrict num, integer_t integer)
{
	if(num->sign == NUM_SIGN_NEGATIVE)
	{
		if(integer < 0)
		{
            integer = abs_integer(integer);

            if(num->int_part < integer)
            {
                return NUM_GREATER;
            }
            else if(num->int_part > integer)
            {
                return NUM_LESSER;
            }

            if(num->numerator == 0)
            {
                return NUM_EQUAL;
            }
		}
        return NUM_LESSER;
	}

    if(num->int_part > integer)
    {
        return NUM_GREATER;
    }
    else if(num->int_part < integer)
    {
        return NUM_LESSER;
    }

    if(num->numerator != 0)
    {
        return NUM_GREATER;
    }
    return NUM_EQUAL;
}

//Greatest common divisor
//I love you, Euclid's algorithm
uinteger_t get_gcd(uinteger_t num1, uinteger_t num2)
{
    uinteger_t dividend = num1, divisor = num2, remainder;
    while(divisor != 0)
    {
        remainder = dividend % divisor;
        dividend = divisor;
        divisor = remainder;
    }
    return dividend;
}

//Least common multiple
uinteger_t get_lcm(uinteger_t num1, uinteger_t num2)
{
    return num1*num2 / get_gcd(num1,num2);
}

static inline void reduce_uints(uinteger_t * const restrict num1, uinteger_t * restrict const num2)
{
    uinteger_t dividend, divisor, remainder;
    while(1)
    {
        dividend = *num1;
        divisor = *num2;
        while(divisor != 0)
        {
            remainder = dividend % divisor;
            dividend = divisor;
            divisor = remainder;
        }
        if(dividend == 1)
        {
            break;
        }
        *num1 /= dividend;
        *num2 /= dividend;
    }
}

inline void simplify_num(num_t * const restrict num)
{
    num->int_part += num->numerator / num->denominator;
    num->numerator = num->numerator % num->denominator;
    if(num->numerator == 0)
    {
        num->denominator = 1;
        num->state = NUM_STATE_INT;
        if(num->int_part == 0)
        {
            num->sign = NUM_SIGN_POSITIVE;
        }
    }
    else
    {
        reduce_uints(&num->numerator, &num->denominator);
        switch (num->state)
        {
            case NUM_STATE_INT:
                num->state = NUM_STATE_FRACTION;
                break;
            case NUM_STATE_DECIMAL:
                if(num->denominator > 999999999)
                {
                    uinteger_t divisor = 10;
                    {
                        uinteger_t denominator = num->denominator/1000000000;
                        while((denominator/=10) != 0)
                        {
                            divisor *= 10;
                        }
                    }
                    num->numerator /= divisor;
                    num->denominator /= divisor;
                }
                break;
        }
    }
}

inline void sum_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product)
{
    //just add
    if(is_num_zero((*num1)))
    {
        *product = *num2;
        return;
    }
    if(is_num_zero((*num2)))
    {
        *product = *num1;
        return;
    }

    if(num1->sign == num2->sign)
    {
        product->sign = num1->sign;
        product->int_part = num1->int_part + num2->int_part;

        if(num1->state != NUM_STATE_INT || num2->state != NUM_STATE_INT)
        {
            if(num1->denominator == num2->denominator)
            {
                product->numerator = num1->numerator + num2->numerator;
                product->denominator = num1->denominator;
            }
            else
            {
                product->denominator = get_lcm(num1->denominator,num2->denominator);
                product->numerator = num1->numerator*(product->denominator/num1->denominator) + num2->numerator*(product->denominator/num2->denominator);
            }
        }
        else
        {
            product->numerator = 0;
            product->denominator = 1;
        }
    }//they are the oppsitive of each other
    else if(num1->state == num2->state && num1->int_part == num2->int_part && num1->denominator == num2->denominator && num1->numerator == num2->numerator)
    {
        product->int_part = 0;
        product->numerator = 0;
        product->denominator = 1;
        product->sign = NUM_SIGN_POSITIVE;
    }//different signs, different values
    else
    {
        //which has the greater abs?
        compare_num_returnv_t comp_returnv;
        {
            num_t num_copy;
            if(num1->sign == NUM_SIGN_NEGATIVE)
            {
                num_copy = *num1;
                num_copy.sign = NUM_SIGN_POSITIVE;
                comp_returnv = compare_num(&num_copy,num2);
            }
            else
            {
                num_copy = *num2;
                num_copy.sign = NUM_SIGN_POSITIVE;
                comp_returnv = compare_num(num1,&num_copy);
            }
        }

        switch (comp_returnv)
        {
            case NUM_LESSER:
                product->sign = num2->sign;

                product->int_part = num2->int_part - num1->int_part;
                //do we have to rest the fractions?
                if(num1->state != NUM_STATE_INT || num2->state != NUM_STATE_INT)
                {
                    //homogeneous fractions
                    if(num1->denominator == num2->denominator)
                    {
                        product->denominator = num1->denominator;
                        if(num1->numerator > num2->numerator)
                        {
                            product->numerator = num1->numerator - num2->numerator;
                            product->int_part--;
                            product->numerator = product->denominator - product->numerator;
                        }
                        else
                        {
                            product->numerator = num2->numerator - num1->numerator;
                        }
                    }
                    else//they are not homogeneous
                    {
                        product->denominator = get_lcm(num1->denominator,num2->denominator);
                        
                        uinteger_t num2_numerator = num2->numerator*(product->denominator/num2->denominator);
                        uinteger_t num1_numerator = num1->numerator*(product->denominator/num1->denominator);
                        if(num1_numerator > num2_numerator)
                        {
                            product->numerator = num1_numerator - num2_numerator;
                            product->int_part--;
                            product->numerator = product->denominator - product->numerator;
                        }
                        else
                        {
                            product->numerator = num2_numerator - num1_numerator;
                        }
                    }
                }
                else
                {
                    product->numerator = 0;
                    product->denominator = 1;
                }
                break;
            case NUM_GREATER:
                product->sign = num1->sign;

                product->int_part = num1->int_part - num2->int_part;
                //do we have to rest the fractions?
                if(num1->state != NUM_STATE_INT || num2->state != NUM_STATE_INT)
                {
                    //homogeneous fractions
                    if(num1->denominator == num2->denominator)
                    {
                        product->denominator = num1->denominator;
                        if(num2->numerator > num1->numerator)
                        {
                            product->numerator = num2->numerator - num1->numerator;
                            product->int_part--;
                            product->numerator = product->denominator - product->numerator;
                        }
                        else
                        {
                            product->numerator = num1->numerator - num2->numerator;
                        }
                    }
                    else//they are not homogeneous
                    {
                        product->denominator = get_lcm(num1->denominator,num2->denominator);

                        uinteger_t num1_numerator = num1->numerator*(product->denominator/num1->denominator);
                        uinteger_t num2_numerator = num2->numerator*(product->denominator/num2->denominator);
                        if(num2_numerator > num1_numerator)
                        {
                            product->numerator = num2_numerator - num1_numerator;
                            product->int_part--;
                            product->numerator = product->denominator - product->numerator;
                        }
                        else
                        {
                            product->numerator = num1_numerator - num2_numerator;
                        }
                    }
                }
                else
                {
                    product->numerator = 0;
                    product->denominator = 1;
                }
                break;
            case NUM_EQUAL:
                product->sign = NUM_SIGN_POSITIVE;
                product->numerator = 0;
                product->denominator = 1;
        }
    }

    if(num2->state == NUM_STATE_DECIMAL)
    {
        product->state = NUM_STATE_DECIMAL;
    }
    else
    {
        product->state = num1->state;
    }

    simplify_num(product);
}

inline void rest_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product)
{
    num_t subtrahend = *num2;
    subtrahend.sign = (subtrahend.sign == NUM_SIGN_NEGATIVE ? NUM_SIGN_POSITIVE : NUM_SIGN_NEGATIVE);
    sum_num(num1,&subtrahend,product);
}

inline void divide_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product)
{
    if(is_num_zero((*num1)))
    {
        *product = INITIALIZER_NUM;
    }
    else
    {
        //A a/b / B c/d
        //(A + a/b) / (B + c/d)

        //(B + c/d)

        num_t divisor;
        divisor.int_part = 0;
        divisor.numerator = num2->denominator;
        divisor.denominator = num2->numerator + num2->int_part*num2->denominator;
        divisor.sign = num2->sign;
        divisor.state = NUM_STATE_FRACTION;

        multiply_num(num1,&divisor,product);
    }   
}

inline void multiply_num(const num_t * const num1, const num_t * const num2, num_t * const restrict product)
{
    if(is_num_zero((*num1)) || is_num_zero((*num2)))
    {
        *product = INITIALIZER_NUM;
        return;
    }

    //+*+ --> +
    //-*- --> +
    if(num1->sign == num2->sign)
    {
        product->sign = NUM_SIGN_POSITIVE;
    }
    else
    {
        //-*+ == -
        //+*- == -
        product->sign = NUM_SIGN_NEGATIVE;
    }

    if(num1->state != NUM_STATE_INT || num2->state != NUM_STATE_INT)
    {
        //supposing that A a/b == num1 and B b/c == num2
        //A a/b * B c/d
        //(A + a/b) * (B + c/d)
        //A*B + A*c/d + B*a/b + a*c/b*d
        //A*B + (A*c/d + B*a/b + a*c/b*d)
        //A*B + Z x/y
        //(A*B + Z) + x/y

        //A*B
        product->int_part = num1->int_part*num2->int_part;

        fraction_t total = {.numerator = 0, .denominator = 1};//it will become x/y
        fraction_t fractions[3];//A*c/d + B*a/b + a*c/b*d

        //A*c/d == num1->int_part * num2->numerator/num2->denominator
        fractions[0] = (fraction_t){.numerator = num1->int_part, .denominator = num2->denominator};
        reduce_uints(&fractions[0].numerator,&fractions[0].denominator);
        fractions[0].numerator *= num2->numerator;

        //B*a/b == num2->int_part * num1->numerator/num1->denominator
        fractions[1] = (fraction_t){.numerator = num2->int_part, .denominator = num1->denominator};
        reduce_uints(&fractions[1].numerator,&fractions[1].denominator);
        fractions[1].numerator *= num1->numerator;

        //a*c/b*d == num1->numerator * num2->numerator / num1->denominator * num2->denominator
        {
            fraction_t aux = {.numerator = num1->numerator, .denominator = num2->denominator};
            fractions[2] = (fraction_t){.numerator = num2->numerator, .denominator = num1->denominator};

            reduce_uints(&aux.numerator,&aux.denominator);
            reduce_uints(&fractions[2].numerator,&fractions[2].denominator);

            fractions[2].numerator *= aux.numerator;
            fractions[2].denominator *= aux.denominator;
        }

        //get (A*B + Z)
        //get the int part of each fraction
        for(unsigned char i = 0; i < 3; i++)
        {
            product->int_part += fractions[i].numerator / fractions[i].denominator;
            fractions[i].numerator = fractions[i].numerator % fractions[i].denominator;
            reduce_uints(&fractions[i].numerator,&fractions[i].denominator);
        }

        //get x/y
        //get total of all fractions
        for(unsigned char i = 0; i < 3; i++)
        {
            uinteger_t lcm = get_lcm(total.denominator,fractions[i].denominator);
            total.numerator = total.numerator*(lcm/total.denominator) + fractions[i].numerator*(lcm/fractions[i].denominator);
            total.denominator = lcm;
            reduce_uints(&total.denominator,&total.numerator);
        }

        //x/y
        product->numerator = total.numerator;
        product->denominator = total.denominator;
        
        //at this point fianlly product is equal to (A*B + Z) + x/y

        //if one of them (num1 or num2) is decimal, the product is also a decimal
        if(num2->state == NUM_STATE_DECIMAL)
        {
            product->state = NUM_STATE_DECIMAL;
        }
        else
        {
            product->state = num1->state;
        }
    }
    else
    {
        product->int_part = num1->int_part*num2->int_part;
        product->numerator = 0;
        product->denominator = 1;
        product->state = NUM_STATE_INT;
    }

    simplify_num(product);
}

bool strtonum(const char * restrict str, num_t * const restrict num)
{
    struct
    {
        uinteger_t numerator, denominator;
    }divisor, dividend;
    
    while(isspace(*str))
    {
        str++;
    }

    switch(*str)
    {
        case '-':
            str++;
            num->sign = NUM_SIGN_NEGATIVE;
            break;
        case '+':
            str++;
        default:
            num->sign = NUM_SIGN_POSITIVE;
    }
    if(sscanf(str,"%"PRIu64,&dividend.numerator) < 1)
    {
        return false;
    }
    while(isdigit(*str))
    {
        str++;
    }
    if(*str == '.')
    {
        str++;
        num->state = NUM_STATE_DECIMAL;

        uinteger_t decimal;
        if(sscanf(str,"%"PRIu64,&decimal) < 1)
        {
            return false;
        }
        while(isdigit(*str))
        {
            str++;
        }
        dividend.denominator = 10;
        for(uinteger_t deci = decimal; (deci/=10) != 0; dividend.denominator*=10);
        dividend.numerator *= dividend.denominator;
        dividend.numerator += decimal; 
        reduce_uints(&dividend.numerator,&dividend.denominator);
    }
    else
    {
        num->state = NUM_STATE_INT;
        dividend.denominator = 1;
    }

    if(*str == '/')
    {
        str++;
        if(num->state != NUM_STATE_DECIMAL)
        {
            num->state = NUM_STATE_FRACTION;
        }

        switch(*str)
        {
            case '-':
                str++;
                num->sign = (num->sign == NUM_SIGN_POSITIVE ? NUM_SIGN_NEGATIVE : NUM_SIGN_POSITIVE);
                break;
            case '+':
                str++;
        }
        if(sscanf(str,"%"PRIu64,&divisor.numerator) < 1)
        {
            return false;
        }
        while(isdigit(*str))
        {
            str++;
        }
        if(*str == '.')
        {
            str++;
            num->state = NUM_STATE_DECIMAL;

            uinteger_t decimal;
            if(sscanf(str,"%"PRIu64,&decimal) < 1)
            {
                return false;
            }
            while(isdigit(*str))
            {
                str++;
            }
            divisor.denominator = 10;
            for(uinteger_t deci = decimal; (deci/=10) != 0; divisor.denominator*=10);
            divisor.numerator *= divisor.denominator;
            divisor.numerator += decimal;
            reduce_uints(&divisor.numerator,&divisor.denominator);
        }
        else
        {
            divisor.denominator = 1;
        }
    }
    else
    {
        divisor.numerator = 1;
        divisor.denominator = 1;
    }

    while(isspace(*str))
    {
        str++;
    }
    if(*str != '\0')
    {
        return false;
    }

    reduce_uints(&dividend.numerator,&divisor.numerator);
    reduce_uints(&dividend.denominator,&divisor.denominator);

    num->numerator = dividend.numerator*divisor.denominator;
    num->denominator = dividend.denominator*divisor.numerator;
    num->int_part = 0;

    simplify_num(num);

    return true;
}

size_t printnum(char * restrict str, const num_t * const restrict num)
{
    size_t str_len = 0;

    if(num->sign == NUM_SIGN_NEGATIVE)
    {
        *str = '-';
        str++;
        str_len++;
    }

    switch (num->state)
    {
    case NUM_STATE_INT:
        sprintf(str,"%"PRIu64,num->int_part);
        break;
    case NUM_STATE_FRACTION:
        sprintf(str,"%"PRIu64"/%"PRIu64,num->int_part*num->denominator + num->numerator, num->denominator);
        break;
    case NUM_STATE_DECIMAL:
        if(num->numerator != 0)
        {
            uinteger_t digit1 = (num->numerator*10) / num->denominator;
            uinteger_t digit2 = (((num->numerator*10) % num->denominator) * 10) / num->denominator;

            if(digit1 != 0)
            {
                if(digit2 != 0)
                {
                    sprintf(str,"%"PRIu64".%"PRIu64,num->int_part,digit1*10+digit2);
                }
                else
                {
                    sprintf(str,"%"PRIu64".%"PRIu64,num->int_part,digit1);
                }
            }
            else
            {
                if(digit2 != 0)
                {
                    sprintf(str,"%"PRIu64".0""%"PRIu64,num->int_part,digit2);
                }
                else
                {
                    sprintf(str,"%"PRIu64,num->int_part);
                }
            }
        }
        break;
    }

    while(*str != '\0')
    {
        str++;
        str_len++;
    }

    return str_len;
}
