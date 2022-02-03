#include "resolve_matrix.h"

size_t get_textlen_in_edit(short_text_t * text)
{
	size_t len = 0;

	for(size_t i = 0; text->str[i] != '\0'; i++)
	{
		if(text->str[i] == ' ' || text->str[i] == '/' || text->str[i] == '.' || text->str[i] == '-' || text->str[i] == ')' || text->str[i] == '(' || text->str[i] == 't')
		{
			len++;
		}
		else if(text->str[i] == 'M')
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

text_t print_matrix_in_edit(const char * name, const char * space, num_t * denominator, matrix_t matrix)
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
					num_len = get_textlen_in_edit(&text_matrix_nums[i][j]);
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

	text_t result = {};//1
    size_t dif;

	for(size_t i = 0; i < matrix.nrows; i++)
	{
			if(i == matrix.nrows/2)
			{
					cat_str_in_text(result,name);
			}
			else
			{
					cat_str_in_text(result,space);
			}

			add_ch_in_text(result,'|');
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
					add_ch_in_text(result,' ');

					num_len = get_textlen_in_edit(&text_matrix_nums[i][j]);
					dif = max_num_len - num_len;

					for(size_t k = 0, max = (dif == 1? 1 : dif/2 + dif%2); k < max; k++)
					{
						add_ch_in_text(result,' ');
					}

					cat_text_in_text(result,text_matrix_nums[i][j]);

					for(size_t k = 0, max = dif/2; k < max; k++)
					{
						add_ch_in_text(result,' ');
					}

					add_ch_in_text(result,' ');
			}

			add_ch_in_text(result,'|');

			add_ch_in_text(result,'\n');
	}

	if(denominator != NULL)
	{
		cat_str_in_text(result,space);

	        line_len = 2+matrix.ncolumns*(max_num_len+2);
		for(size_t i = 0; i < line_len; i++)
	        {
	                add_ch_in_text(result,'-');
	        }
		add_ch_in_text(result,'\n');

		cat_str_in_text(result,space);

		short_text_t text_denominator = {0};
		print_num_in_text(text_denominator,(*denominator));

                num_len = get_textlen_in_edit(&text_denominator);
                dif = line_len - num_len;

		for(size_t i = 0, max = dif/2 + dif%2 ; i < max; i++)
		{
			add_ch_in_text(result,' ');
		}

		cat_text_in_text(result,text_denominator);

		for(size_t i = 0, max = dif/2; i < max; i++)
		{
			add_ch_in_text(result,' ');
		}
	}

    return result;
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

num_t get_det(const char * name, num_t * denominator, matrix_t matrix, text_t * process)
{
	num_t nums[NXN_MAXN*6];
	num_t nums_product[NXN_MAXN*2];
	for(size_t i = 0; i < NXN_MAXN*2; i++)
	{
		set_num(nums_product[i],1,1);
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
		denominator_len = get_textlen_in_edit(&buffer);
		buffer = create_short_text(name);
		name_len = get_textlen_in_edit(&buffer);
	}

	for(size_t k = 0; k < 2; k++)
	{
		cat_str_in_text((*process),name);

		if(denominator != NULL)
		{
			line_len = get_textlen_in_edit(&text_to_short(*process));
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
				add_ch_in_text((*process),'\n');
			}
		}
		if(denominator != NULL)
		{
			line_len = get_textlen_in_edit(&text_to_short(*process))-line_len;
			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}
			for(size_t i = 0; i < line_len; i++)
			{
				add_ch_in_text((*process),'-');
			}

			add_ch_in_text((*process),'\n');

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
			add_ch_in_text((*process),'\n');
		}
		add_ch_in_text((*process),'\n');
	}

	if(matrix.is_3x3)
	{
		cat_str_in_text((*process),name);

		if(denominator != NULL)
		{
			line_len = get_textlen_in_edit(&text_to_short(*process));
		}

		for(size_t i = 0; i < 2; i++)
		{
			add_ch_in_text((*process),'{');
			print_num_in_text((*process),nums_sum[i]);
			add_ch_in_text((*process),'}');
			add_ch_in_text((*process),(i != 1? '-' : '\n'));
		}

		if(denominator != NULL)
		{
			line_len = get_textlen_in_edit(&text_to_short(*process))-line_len;
			for(size_t i = 0; i < name_len; i++)
			{
				add_ch_in_text((*process),' ');
			}
			for(size_t i = 0; i < line_len; i++)
			{
				add_ch_in_text((*process),'-');
			}
			add_ch_in_text((*process),'\n');


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
			add_ch_in_text((*process),'\n');
		}
		add_ch_in_text((*process),'\n');
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
				add_ch_in_text(text,'\n');
			}
		}
	}
	return text;
}

#define BOTTOM_RIGHT_CORNER 217
#define BOTTOM_LEFT_CORNER 192

#define TOP_RIGHT_CORNER 191
#define TOP_LEFT_CORNER 218

text_t cramer(matrix_t matrix)
{
	text_t text = {};//1
	text_t aux_text = {};//2
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

	text = print_matrix_in_edit("det Ms = ","               ",NULL,matrix);

	add_ch_in_text(text,'\n');
	sys_det = get_det("det Ms = ",NULL,matrix,&text);
	cat_str_in_text(text,"\n\n\n\n");

	if(get_num(sys_det) == 0)
	{
		cat_str_in_text(text,"\nNo se puede resolver el sistema pues la determinante del sistema es cero.");
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
		

		aux_text = print_matrix_in_edit(name,"      ",&sys_det,matrix);//2
		printf("text.len: %zu, text.allocated_bytes: %zu\n",text.len,text.allocated_bytes);
		printf("aux_text.len: %zu, aux_text.allocated_bytes: %zu\n",aux_text.len,aux_text.allocated_bytes);

		cat_text_in_text(text,aux_text);
		clear_text(aux_text);//2

		cat_str_in_text(text,"\n\n");
		solution[i] = simplify_num(divide_num(get_det(name,&sys_det,matrix,&text),sys_det));
		cat_str_in_text(text," = ");
		print_num_in_text(text,solution[i]);
		cat_str_in_text(text,"\n\n\n\n");

		name[0]++;

		for(size_t j = 0; j < matrix.nrows; j++)
		{
			num_t aux = matrix.matrix[j][i];
			matrix.matrix[j][i] = matrix.matrix[j][matrix.ncolumns];
			matrix.matrix[j][matrix.ncolumns] = aux;
		}
	}

	cat_str_in_text(text,"\nSolucion: (");
	for(size_t i = 0; i < matrix.ncolumns; i++)
	{
		print_num_in_text(text,solution[i]);
		if(i+1 != matrix.ncolumns)
		{
			cat_str_in_text(text," ; ");
		}
	}
	cat_str_in_text(text,")\n");

	return text;//1
}

text_t gauss(matrix_t matrix)
{
	text_t text = {};
	return text;
}

text_t jordan(matrix_t matrix)
{
	text_t text = {};
	return text;
}
