#include "matrix.h"

void matrix_to_system(matrix_t matrix, text_t * text)
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
        bool is_negative_last_elem = matrix.matrix[i][maxj].numerator < 0;
        for(j = 0; j < maxj; j++)
        {
            if(matrix.matrix[i][j].numerator == 0)
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
            if(matrix.matrix[i][no_zero_index].numerator == -1)
            {
                add_ch_in_text((*text),'-');
            }
            else if(matrix.matrix[i][no_zero_index].numerator != 1)
            {
                print_num_in_text((*text),matrix.matrix[i][no_zero_index]);
            }
            add_ch_in_text((*text),'x'+no_zero_index);
        }
        else
        {
            for(j = 0; j < maxj; j++)
            {
                num_t num = simplify_num(matrix.matrix[i][j]);
                if(num.numerator != 0)
                {
                    if(num.numerator != 1)
                    {
                        print_num_in_text((*text),num);
                    }
                    add_ch_in_text((*text),'x'+j);
                }
                if(j != maxj-1)
                {
                    if(matrix.matrix[i][j+1].numerator < 0)
                    {
                        cat_str_in_text((*text)," - ");
                    }
                    else if(matrix.matrix[i][j+1].numerator != 0)
                    {
                        cat_str_in_text((*text)," + ");
                    }
                }
                matrix.matrix[i][j+1] = abs_num(matrix.matrix[i][j+1]);
            }
        }

        matrix.matrix[i][matrix.ncolumns-1] = simplify_num(matrix.matrix[i][matrix.ncolumns-1]);
        cat_str_in_text((*text)," = ");
        if(is_negative_last_elem)
        {
            add_ch_in_text((*text),'-');
        }
        print_num_in_text((*text),matrix.matrix[i][matrix.ncolumns-1]);
        cat_str_in_text((*text),"\r\n");
	}
}
