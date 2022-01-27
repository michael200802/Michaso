#include "resolve_matrix.h"

text_t matrix_to_system(matrix_t matrix)
{
	text_t text = TEXT_INITIALIZER;
	for(size_t i = 0; i < matrix.nrows; i++)
	{
		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			matrix.matrix[i][j] = simplify_num(matrix.matrix[i][j]);
			print_num_in_text(text,matrix.matrix[i][j]);
			print_in_text(text,"%c%s", j != matrix.ncolumns-1 ? 'x'+j : ' ', j != matrix.ncolumns-1 ? (j == matrix.ncolumns-2 ? " = " : " + ") : "\n");
		}
	}
	return text;
}

text_t cramer(matrix_t matrix)
{
	text_t text = create_text();
	for(size_t i = 0; i < matrix.nrows; i++)
	{
		if(i == matrix.nrows/2)
		{
			cat_str_in_text(text,"det Ms = ");
		}
		else
		{
			cat_str_in_text(text,"               ");
		}

		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			print_num_in_text(text,matrix.matrix[i][j]);
			add_ch_in_text(text,' ');
		}

		add_ch_in_text(text,'\n');
	}
	return text;
}

text_t gauss(matrix_t matrix)
{
	text_t text = create_text();
	return text;
}

text_t jordan(matrix_t matrix)
{
	text_t text = create_text();
	return text;
}
