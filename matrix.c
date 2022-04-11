#include "matrix.h"

void matrix_to_system(matrix_t matrix, text_t * const restrict text)
{
	if(matrix.is_3x3)
	{
		matrix.nrows = 3;
		matrix.ncolumns = 4;
	}

	for(size_t i = 0; i < matrix.nrows; i++)
	{
        size_t nzeros = 0, no_zero_index;
        size_t j, maxj = matrix.ncolumns-1;
        bool is_negative_last_elem = matrix.matrix[i][maxj].sign == NUM_SIGN_NEGATIVE;
        for(j = 0; j < maxj; j++)
        {
            if(is_num_zero(matrix.matrix[i][j]))
            {
                nzeros++;
            }
            else
            {
                no_zero_index = j;
            }
        }

        if(nzeros == maxj)
        {
            add_ch_in_text((*text),'0');
        }
        else if(maxj-nzeros == 1)
        {
            if(is_num_minus1(matrix.matrix[i][no_zero_index]))
            {
                add_ch_in_text((*text),'-');
            }
            else if(!is_num_one(matrix.matrix[i][no_zero_index]))
            {
                print_num_in_text((*text),matrix.matrix[i][no_zero_index]);
            }
            add_ch_in_text((*text),'x'+no_zero_index);
        }
        else
        {
            for(j = 0; j < maxj; j++)
            {
                num_t num = matrix.matrix[i][j];

                if(!is_num_zero(num))
                {
                    if(!is_num_one(num))
                    {
                        print_num_in_text((*text),num);
                    }
                    add_ch_in_text((*text),'x'+j);
                }
                if(j != maxj-1)
                {
                    if(matrix.matrix[i][j+1].sign == NUM_SIGN_NEGATIVE)
                    {
                        cat_str_in_text((*text)," - ");
                    }
                    else if(!is_num_zero(matrix.matrix[i][j+1]))
                    {
                        cat_str_in_text((*text)," + ");
                    }
                }
                abs_num(matrix.matrix[i][j+1]);
            }
        }

        cat_str_in_text((*text)," = ");
        if(is_negative_last_elem)
        {
            add_ch_in_text((*text),'-');
        }
        print_num_in_text((*text),matrix.matrix[i][matrix.ncolumns-1]);
        cat_str_in_text((*text),"\r\n");
	}
}
