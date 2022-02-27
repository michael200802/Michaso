#include "resolve_matrix.h"

text_t matrix_to_system(matrix_t matrix)
{
	if(matrix.is_3x3)
	{
		matrix.nrows = 3;
		matrix.ncolumns = 4;
	}

	text_t text = {};
	for(size_t i = 0; i < matrix.nrows; i++)
	{
		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			matrix.matrix[i][j] = simplify_num(matrix.matrix[i][j]);

			num_t num;
			if(j != 0 && j != matrix.ncolumns-1)
			{
				if(get_num(matrix.matrix[i][j]) < 0)
				{
					cat_str_in_text(text, "- ");
				}
				else
				{
					cat_str_in_text(text, "+ ");
				}
				num = abs_num(matrix.matrix[i][j]);
			}
			else
			{
				num = matrix.matrix[i][j];
			}
			print_num_in_text(text,num);
			if(j != matrix.ncolumns-1)
			{
				add_ch_in_text(text,'x'+j);
				if(j == matrix.ncolumns-2)
				{
					cat_str_in_text(text," = ");
				}
				else
				{
					add_ch_in_text(text,' ');
				}
			}
			else
			{
				cat_str_in_text(text,"\r\n");
			}
		}
	}
	return text;
}

size_t get_strlen_in_edit(const char * str)
{
	size_t len = 0;

	for(size_t i = 0; str[i] != '\0'; i++)
	{
		if(str[i] == ' ' || str[i] == '/' || str[i] == '.' || str[i] == '-' || str[i] == ')' || str[i] == '(' || str[i] == 't')
		{
			len++;
		}
		else if(str[i] == 'M')
		{
			len+=3;
		}
		else
		{
			len+=2;
		}
	}

	return len;
}

void show_numarray(num_t * array, size_t len)
{
	short_text_t text = {};
	for(size_t i = 0; i < len; i++)
	{
		print_num_in_text(text,array[i]);
		printf("%s|",text.str);
		clear_text(text);
	}
	putchar('\n');
}

//cramer

//old
void _print_matrix_in_edit(text_t * edit, const char * name, const char * space, num_t * denominator, matrix_t matrix)
{
	short_text_t text_matrix_nums[NXN_MAXN*2-1][NXN_MAXN+1] = {0};
	size_t max_line_len = 0, line_len;
	size_t max_num_len = 0, num_len;
	for(size_t i = 0; i < matrix.nrows; i++)
	{
			line_len = 0;
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
					matrix.matrix[i][j] = simplify_num(matrix.matrix[i][j]);
					print_num_in_text(text_matrix_nums[i][j],matrix.matrix[i][j]);
					num_len = get_strlen_in_edit(text_matrix_nums[i][j].str);
					if(max_num_len < num_len)
					{
							max_num_len = num_len;
					}
					line_len += num_len+2;
			}
			if(max_line_len < line_len)
			{
					max_line_len = line_len;
			}
	}

    	size_t dif;

	for(size_t i = 0; i < matrix.nrows; i++)
	{
			if(i == matrix.nrows/2)
			{
					cat_str_in_text((*edit),name);
			}
			else
			{
					cat_str_in_text((*edit),space);
			}

			add_ch_in_text((*edit),'|');
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
					add_ch_in_text((*edit),' ');

					num_len = get_strlen_in_edit(text_matrix_nums[i][j].str);
					dif = max_num_len - num_len;

					for(size_t k = 0, max = (dif == 1? 1 : dif/2 + dif%2); k < max; k++)
					{
						add_ch_in_text((*edit),' ');
					}

					cat_text_in_text((*edit),text_matrix_nums[i][j]);

					for(size_t k = 0, max = dif/2; k < max; k++)
					{
						add_ch_in_text((*edit),' ');
					}

					add_ch_in_text((*edit),' ');
			}

			cat_str_in_text((*edit),"|\r\n");
	}

	if(denominator != NULL)
	{
		cat_str_in_text((*edit),space);

	        line_len = 2+matrix.ncolumns*(max_num_len+2);
		for(size_t i = 0; i < line_len; i++)
	        {
	                add_ch_in_text((*edit),'-');
	        }
		cat_str_in_text((*edit),"\r\n");

		cat_str_in_text((*edit),space);

		short_text_t text_denominator = {0};
		print_num_in_text(text_denominator,(*denominator));

                num_len = get_strlen_in_edit(text_denominator.str);
                dif = line_len - num_len;

		for(size_t i = 0, max = dif/2 + dif%2 ; i < max; i++)
		{
			add_ch_in_text((*edit),' ');
		}

		cat_text_in_text((*edit),text_denominator);

		for(size_t i = 0, max = dif/2; i < max; i++)
		{
			add_ch_in_text((*edit),' ');
		}
	}
}

//new
void print_matrix_in_edit(text_t * edit, const char * name, const char * space, num_t * denominator, matrix_t matrix)
{
	short_text_t text_matrix_nums[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS] = {0};
	size_t matrix_nums_len[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
	size_t max_line_len = 2;
	size_t matrix_nums_col_max_num[MATRIX_MAXNCOLUMNS];
	size_t max_num_len = 0, num_len;
	num_t cur_num;
   	size_t dif;

	for(size_t i = 0; i < matrix.nrows; i++)
	{
		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			cur_num = matrix.matrix[i][j];
			cur_num = simplify_num(cur_num);
			
			print_num_in_text(text_matrix_nums[i][j], cur_num);
			num_len = get_strlen_in_edit(text_matrix_nums[i][j].str);

			matrix_nums_len[i][j] = num_len;
		}
	}

	for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
	{
		max_num_len = 0;
		for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
		{
			num_len = matrix_nums_len[cur_row][cur_col];
			if(max_num_len < num_len)
			{
				max_num_len = num_len;
			}
		}
		matrix_nums_col_max_num[cur_col] = max_num_len;
		max_line_len += max_num_len+2;
	}

	for(size_t i = 0; i < matrix.nrows; i++)
	{
		if(i == matrix.nrows/2)
		{
			cat_str_in_text((*edit),name);
		}
		else
		{
			cat_str_in_text((*edit),space);
		}

		add_ch_in_text((*edit),'|');
		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			num_len = matrix_nums_len[i][j];
			dif = matrix_nums_col_max_num[j] - num_len;

			for(size_t k = 0, max = (dif/2 + dif%2) + 1; k < max; k++)
			{
				add_ch_in_text((*edit),' ');
			}

			cat_text_in_text((*edit),text_matrix_nums[i][j]);

			for(size_t k = 0, max = (dif/2) + 1; k < max; k++)
			{
				add_ch_in_text((*edit),' ');
			}
		}

		cat_str_in_text((*edit),"|\r\n");
	}

	if(denominator != NULL)
	{
		cat_str_in_text((*edit),space);

	    //line_len = 2;
		for(size_t i = 0; i < max_line_len; i++)
	    {
	        add_ch_in_text((*edit),'-');
	    }
		cat_str_in_text((*edit),"\r\n");

		cat_str_in_text((*edit),space);

		short_text_t text_denominator = {0};
		print_num_in_text(text_denominator,(*denominator));

		num_len = get_strlen_in_edit(text_denominator.str);
		dif = max_line_len - num_len;

		for(size_t i = 0, max = dif/2 + dif%2 ; i < max; i++)
		{
			add_ch_in_text((*edit),' ');
		}

		cat_text_in_text((*edit),text_denominator);

		for(size_t i = 0, max = dif/2; i < max; i++)
		{
			add_ch_in_text((*edit),' ');
		}
	}
}

num_t get_det(const char * name, num_t * denominator, matrix_t matrix, text_t * process)
{
	num_t nums[NXN_MAXN*6];
	num_t nums_product[NXN_MAXN*2];
	for(size_t i = 0; i < NXN_MAXN*2; i++)
	{
		set_num(nums_product[i],1,1,NUM_STATE_INT);
	}
	num_t nums_sum[2] = {INITIALIZER_NUM,INITIALIZER_NUM};
	num_t det = INITIALIZER_NUM;
	size_t nums_con = 0;
	size_t nums_product_con = 0;

	size_t line_len, name_len;
	size_t denominator_len;

	//fill nums
	{
		size_t i = 0;
		for(size_t max = (matrix.is_3x3? 2:0); i <= max; i++)
		{
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
				nums[nums_con] = matrix.matrix[i+j][j];
				nums_con++;
			}
		}
		if(matrix.is_3x3)
		{
			i--;
		}
		for(; i < matrix.nrows; i++)
		{
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
				nums[nums_con] = matrix.matrix[i-j][j];
				nums_con++;
			}
		}
	}

	//fill nums_product with nums
	for(size_t i = 0, j = 0, jmax = (matrix.is_3x3? 3 : 2); i < nums_con; i++, j++)
	{
		if(j == jmax)
		{
			nums_product_con++;
			j = 0;
		}
		nums_product[nums_product_con] = multiply_num(nums_product[nums_product_con],nums[i]);
	}
	nums_product_con++;


	//fill nums_sum with nums_product
	for(size_t i = 0, j = 0; i < nums_product_con; i++)
	{
		if(i == nums_product_con/2)
		{
			j++;
		}
		nums_sum[j] = sum_num(nums_sum[j],nums_product[i]);
	}

	//get det
	det = rest_num(nums_sum[0],nums_sum[1]);

	if(denominator != NULL)
	{
		short_text_t buffer = {};

		print_num_in_text(buffer,(*denominator));
		denominator_len = get_strlen_in_edit(buffer.str);
		buffer = create_short_text(name);
		name_len = get_strlen_in_edit(buffer.str);
	}

	for(size_t k = 0; k < 2; k++)
	{
		cat_str_in_text((*process),name);

		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit(process->str);
		}

		for(size_t m = 0, i = 0; m < 2; m++)
		{
			add_ch_in_text((*process),'{');
			switch(k)
			{
				case 0:
					for(size_t imax = (m == 0 ? nums_con/2 : nums_con); i < imax;)
					{
						add_ch_in_text((*process),(matrix.is_3x3 ? '(':' '));
						for(size_t j = 0, jmax = (matrix.is_3x3? 3:2); j < jmax; j++, i++)
						{
							print_num_in_text((*process),nums[i]);
							if(j+1 != jmax)
							{
								cat_str_in_text((*process)," * ");
							}
						}
						add_ch_in_text((*process),(matrix.is_3x3 ? ')':' '));

						if(i != imax)
						{
							cat_str_in_text((*process)," + ");
						}
					}

					break;
				case 1:
					for(size_t max = (m == 0? nums_product_con/2 : nums_product_con); i < max; i++)
					{
						print_num_in_text((*process),nums_product[i]);

						if(i+1 != max)
						{
							cat_str_in_text((*process)," + ");
						}
					}
					break;
			}
			add_ch_in_text((*process),'}');

			if(m == 0)
			{
				cat_str_in_text((*process)," - ");
			}
			else
			{
				cat_str_in_text((*process),"\r\n");
			}
		}
		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit(process->str)-line_len;
			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}
			for(size_t i = 0; i < line_len; i++)
			{
				add_ch_in_text((*process),'-');
			}

			cat_str_in_text((*process),"\r\n");

			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}

			size_t dif = (line_len - denominator_len);
			for(size_t i = 0, max = dif/2 + dif%2; i < max; i++)
			{
				add_ch_in_text((*process),' ');
			}
			print_num_in_text((*process),(*denominator));
			for(size_t i = 0, max = dif/2; i < max; i++)
			{
				add_ch_in_text((*process),' ');
			}
			cat_str_in_text((*process),"\r\n");
		}
		cat_str_in_text((*process),"\r\n");
	}

	if(matrix.is_3x3)
	{
		cat_str_in_text((*process),name);

		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit(process->str);
		}

		for(size_t i = 0; i < 2; i++)
		{
			print_num_in_text((*process),nums_sum[i]);
			if(i != 1)
			{
				cat_str_in_text((*process)," - ");
			}
			else
			{
				cat_str_in_text((*process),"\r\n");
			}
		}

		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit(process->str)-line_len;
			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}
			for(size_t i = 0; i < line_len; i++)
			{
				add_ch_in_text((*process),'-');
			}
			cat_str_in_text((*process),"\r\n"); 


			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}

			size_t dif = (line_len - denominator_len);
			for(size_t i = 0, max = dif/2 + dif%2; i < max; i++)
			{
				add_ch_in_text((*process),' ');
			}
			print_num_in_text((*process),(*denominator));
			for(size_t i = 0, max = dif/2; i < max; i++)
			{
				add_ch_in_text((*process),' ');
			}
			cat_str_in_text((*process),"\r\n"); 
		}
		cat_str_in_text((*process),"\r\n"); 
	}

	cat_str_in_text((*process),name);
	print_num_in_text((*process),det);
	if(denominator != NULL)
	{
		add_ch_in_text((*process),'/');
		print_num_in_text((*process),(*denominator));
	}

	return det;
}

text_t cramer(matrix_t matrix)
{
	text_t text = {};//1
	num_t sys_det;
	num_t solution[NXN_MAXN];

	if(matrix.is_3x3)
	{
		for(size_t i = 0; i < 2; i++)
		{
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
				matrix.matrix[i+3][j] = matrix.matrix[i][j];
			}
		}
		matrix.ncolumns = 3;
		matrix.nrows = 5;
	}
	else
	{
		matrix.ncolumns = 2;
		matrix.nrows = 2;
	}

	print_matrix_in_edit(&text,"det Ms = ","               ",NULL,matrix);

	cat_str_in_text(text,"\r\n");
	sys_det = get_det("det Ms = ",NULL,matrix,&text);
	cat_str_in_text(text,"\r\n\r\n\r\n\r\n");

	if(get_num(sys_det) == 0)
	{
		cat_str_in_text(text,"\r\nNo se puede resolver el sistema pues la determinante del sistema es cero.");
		return text;
	}

	char name[] = "x = ";
	for(size_t i = 0; i < matrix.ncolumns; i++)
	{
		for(size_t j = 0; j < matrix.nrows; j++)
		{
			num_t aux = matrix.matrix[j][i];
			matrix.matrix[j][i] = matrix.matrix[j][matrix.ncolumns];
			matrix.matrix[j][matrix.ncolumns] = aux;
		}

		print_matrix_in_edit(&text,name,"      ",&sys_det,matrix);
		printf("text.len: %zu --> text.allocated_bytes: %zu\n",text.len,text.allocated_bytes);

		cat_str_in_text(text,"\r\n\r\n");
		solution[i] = simplify_num(divide_num(get_det(name,&sys_det,matrix,&text),sys_det));
		cat_str_in_text(text," = ");
		print_num_in_text(text,solution[i]);
		cat_str_in_text(text,"\r\n\r\n\r\n\r\n");
		name[0]++;

		for(size_t j = 0; j < matrix.nrows; j++)
		{
			num_t aux = matrix.matrix[j][i];
			matrix.matrix[j][i] = matrix.matrix[j][matrix.ncolumns];
			matrix.matrix[j][matrix.ncolumns] = aux;
		}
	}

	cat_str_in_text(text,"\r\nSolucion: (");
	for(size_t i = 0; i < matrix.ncolumns; i++)
	{
		print_num_in_text(text,solution[i]);
		if(i+1 != matrix.ncolumns)
		{
			cat_str_in_text(text," ; ");
		}
	}
	cat_str_in_text(text,")\r\n");

	return text;//1
}

//cramer

//gauss-jordan

void print_matrix_in_edit_gj(text_t * edit, matrix_t matrix)
{
	short_text_t text_matrix_nums[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS] = {};
	size_t matrix_nums_len[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
	size_t matrix_nums_col_max_num[MATRIX_MAXNCOLUMNS];
	size_t max_num_len = 0, cur_num_len;
	num_t cur_num;
	size_t len_dif;

	for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
	{
		for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
		{
			cur_num = matrix.matrix[cur_row][cur_col];

			matrix.matrix[cur_row][cur_col] = simplify_num(matrix.matrix[cur_row][cur_col]);
			print_num_in_text(text_matrix_nums[cur_row][cur_col], matrix.matrix[cur_row][cur_col]);
			matrix_nums_len[cur_row][cur_col] = get_strlen_in_edit(text_matrix_nums[cur_row][cur_col].str);
		}
	}

	for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
	{
		max_num_len = 0;
		for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
		{
			cur_num_len = matrix_nums_len[cur_row][cur_col];

			if(max_num_len < cur_num_len)
			{
				max_num_len = cur_num_len;
			}
		}
		matrix_nums_col_max_num[cur_col] = max_num_len;
	}

	for (size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
	{
		add_ch_in_text((*edit),'|');
		for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
		{
			if(cur_col == matrix.ncolumns-1)
			{
				add_ch_in_text((*edit),'|');
			}

			cur_num_len = matrix_nums_len[cur_row][cur_col];
			
			len_dif = matrix_nums_col_max_num[cur_col] - cur_num_len;

			for(size_t i = 0, imax = (len_dif/2 + len_dif%2) + 1; i < imax; i++)
			{
				add_ch_in_text((*edit),' ');
			}

			cat_text_in_text((*edit), text_matrix_nums[cur_row][cur_col]);
			
			for(size_t i = 0, imax = (len_dif/2) + 1; i < imax; i++)
			{
				add_ch_in_text((*edit),' ');
			}

		}
		cat_str_in_text((*edit),"|\r\n");
	}
	

}

text_t gauss(matrix_t matrix)
{
	text_t text = {};

	print_matrix_in_edit_gj(&text, matrix);

	return text;
}

text_t gauss_jordan(matrix_t matrix)
{
	text_t text = {};

	print_matrix_in_edit_gj(&text, matrix);

	return text;
}

//gauss-jordan
