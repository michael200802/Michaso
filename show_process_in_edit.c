#include "show_process_in_edit.h"

//edit---------------------------------------------------
static inline void init_edit(hedit_t edit)
{
    Edit_SetText((HWND)edit,"");
}
static inline void cat_str_in_edit(hedit_t edit, const char * str)
{
    size_t textlen = Edit_GetTextLength((HWND)edit);
    Edit_SetSel((HWND)edit,textlen,textlen);
    Edit_ReplaceSel((HWND)edit,str);
}
static inline void cat_text_in_edit(hedit_t edit, const text_t * text)
{
    cat_str_in_edit(edit, text->str);
}
//edit----------------------------------------------------

//get the number of spaces that a string occupies in an edit.
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
//get the number of spaces that a string occupies in an edit

//just for testing
void show_numarray(num_t * array, size_t len)
{
	short_text_t text;
    init_text(text);

	for(size_t i = 0; i < len; i++)
	{
		print_num_in_text(text,array[i]);
        add_ch_in_text(text,'|');
	}

    puts(text.str);
}
//just for testing

//CRAMER------------------------------------------

typedef struct
{
	const char * name;
	const num_t * denominator;
	const matrix_t * matrix;
}get_det_args;

typedef struct
{
	num_t det;
	long_text_t process;
}get_det_return_struct;

//Get det and process to get this det
num_t get_det(const char * name, const num_t * denominator, const matrix_t * matrix, long_text_t * process)
{
	init_text((*process));

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
		for(size_t max = (matrix->is_3x3? 2:0); i <= max; i++)
		{
			for(size_t j = 0; j < matrix->ncolumns; j++)
			{
				nums[nums_con] = matrix->matrix[i+j][j];
				nums_con++;
			}
		}
		if(matrix->is_3x3)
		{
			i--;
		}
		for(; i < matrix->nrows; i++)
		{
			for(size_t j = 0; j < matrix->ncolumns; j++)
			{
				nums[nums_con] = matrix->matrix[i-j][j];
				nums_con++;
			}
		}
	}

	//fill nums_product with nums
	for(size_t i = 0, j = 0, jmax = (matrix->is_3x3? 3 : 2); i < nums_con; i++, j++)
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
		print_num_in_text((*process),(*denominator));
		denominator_len = get_strlen_in_edit(process->str);

		init_text((*process));
		cat_str_in_text((*process),name);
		name_len = get_strlen_in_edit(process->str);
		init_text((*process));
	}

    //show the two first steps
    //1: write the entire operation: {(...) + (...)} - {(...) + (...)}
    //2: show the product: {. + . + .} - {. + . + .}

	for(size_t k = 0; k < 2; k++)
	{
        if(denominator != NULL)
		{
			line_len = get_strlen_in_edit(process->str);
		}

        //print the name
		cat_str_in_text((*process),name);

        //first it shows this part "{}" - {}
        //then it shows this part {} - "{}"
		for(size_t m = 0, i = 0; m < 2; m++)
		{
			add_ch_in_text((*process),'{');
			switch(k)
			{
				case 0://first step
					for(size_t imax = (m == 0 ? nums_con/2 : nums_con); i < imax;)
					{
						add_ch_in_text((*process),(matrix->is_3x3 ? '(':' '));
						for(size_t j = 0, jmax = (matrix->is_3x3? 3:2); j < jmax; j++, i++)
						{
							print_num_in_text((*process),nums[i]);
							if(j+1 != jmax)
							{
								cat_str_in_text((*process)," * ");
							}
						}
						add_ch_in_text((*process),(matrix->is_3x3 ? ')':' '));

						if(i != imax)
						{
							cat_str_in_text((*process)," + ");
						}
					}

					break;
				case 1://second step
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

			if(m == 0)//"{}" - {}
			{
				cat_str_in_text((*process)," - ");
			}
			else//{} - "{}"
			{
				cat_str_in_text((*process),"\r\n");
			}
		}

		if(denominator != NULL)
		{
            //show the denominator
            //{}-{}/denominator

			line_len = get_strlen_in_edit((*process).str) - line_len;

            add_nch_in_text((*process),name_len,' ');
            add_nch_in_text((*process),line_len,'-');

			cat_str_in_text((*process),"\r\n");

            add_nch_in_text((*process),name_len,' ');

			size_t dif = (line_len - denominator_len);
            add_nch_in_text((*process), dif/2 + dif%2, ' ');
			print_num_in_text((*process),(*denominator));
            add_nch_in_text((*process),dif/2,' ');

			cat_str_in_text((*process),"\r\n\r\n");

		}
        else
        {
            cat_str_in_text((*process),"\r\n");
        }
	}

	if(matrix->is_3x3)
	{

		cat_str_in_text((*process),name);

		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit((*process).str);
		}

        print_num_in_text((*process),nums_sum[0]);
        cat_str_in_text((*process)," - ");
        print_num_in_text((*process),nums_sum[1]);
        cat_str_in_text((*process),"\r\n");

		if(denominator != NULL)
		{
			line_len = get_strlen_in_edit((*process).str)-line_len;

            add_nch_in_text((*process),name_len,' ');

            add_nch_in_text((*process),line_len,'-');

			cat_str_in_text((*process),"\r\n");

            add_nch_in_text((*process), name_len, ' ');

			size_t dif = (line_len - denominator_len);

            add_nch_in_text((*process), dif/2 + dif%2, ' ');
			print_num_in_text((*process),(*denominator));
			add_nch_in_text((*process),dif/2,' ');
			
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
//Get det and process to get this det

void * get_det_thread_routine(void * arg)
{
	get_det_args args = *(get_det_args*)arg;
	get_det_return_struct * return_struct = (get_det_return_struct*)malloc(sizeof(get_det_return_struct));

	return_struct->det = get_det(args.name,args.denominator,args.matrix,&return_struct->process);

	return return_struct; 
}

//Get det and show process in edit
num_t get_sysdet(const char * name, matrix_t matrix, hedit_t edit)
{
    text_t buffer;

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

    //show the two first steps
    //1: write the entire operation: {(...) + (...)} - {(...) + (...)}
    //2: show the product: {. + . + .} - {. + . + .}

	for(size_t k = 0; k < 2; k++)
	{
        //init
        init_text(buffer);

        //print the name
		cat_str_in_text(buffer,name);

        //first it shows this part "{}" - {}
        //then it shows this part {} - "{}"
		for(size_t m = 0, i = 0; m < 2; m++)
		{
			add_ch_in_text(buffer,'{');
			switch(k)
			{
				case 0://first step
					for(size_t imax = (m == 0 ? nums_con/2 : nums_con); i < imax;)
					{
						add_ch_in_text(buffer,(matrix.is_3x3 ? '(':' '));
						for(size_t j = 0, jmax = (matrix.is_3x3? 3:2); j < jmax; j++, i++)
						{
							print_num_in_text(buffer,nums[i]);
							if(j+1 != jmax)
							{
								cat_str_in_text(buffer," * ");
							}
						}
						add_ch_in_text(buffer,(matrix.is_3x3 ? ')':' '));

						if(i != imax)
						{
							cat_str_in_text(buffer," + ");
						}
					}

					break;
				case 1://second step
					for(size_t max = (m == 0? nums_product_con/2 : nums_product_con); i < max; i++)
					{
						print_num_in_text(buffer,nums_product[i]);

						if(i+1 != max)
						{
							cat_str_in_text(buffer," + ");
						}
					}
					break;
			}
			add_ch_in_text(buffer,'}');

			if(m == 0)//"{}" - {}
			{
				cat_str_in_text(buffer," - ");
			}
			else//{} - "{}"
			{
				cat_str_in_text(buffer,"\r\n");
			}
		}
		cat_str_in_text(buffer,"\r\n");

		cat_text_in_edit(edit, &buffer);
	}

	if(matrix.is_3x3)
	{
        init_text(buffer);

		cat_str_in_text(buffer,name);

        print_num_in_text(buffer,nums_sum[0]);
        cat_str_in_text(buffer," - ");
        print_num_in_text(buffer,nums_sum[1]);
        cat_str_in_text(buffer,"\r\n");

		cat_str_in_text(buffer,"\r\n");

        cat_text_in_edit(edit, &buffer);
	}

    init_text(buffer);
	cat_str_in_text(buffer,name);
	print_num_in_text(buffer,det);
    cat_text_in_edit(edit, &buffer);

	return det;
}
//get det and show process in edit

//print matrix in cramer
void print_matrix_in_edit_cramer(hedit_t edit, const char * name, const char * space, num_t * denominator, matrix_t matrix)
{
    text_t buffer = {};
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
        init_text(buffer);

		if(i == matrix.nrows/2)
		{
			cat_str_in_text(buffer,name);
		}
		else
		{
			cat_str_in_text(buffer,space);
		}

		add_ch_in_text(buffer,'|');
		for(size_t j = 0; j < matrix.ncolumns; j++)
		{
			num_len = matrix_nums_len[i][j];
			dif = matrix_nums_col_max_num[j] - num_len;

            add_nch_in_text(buffer,(dif/2 + dif%2) + 1,' ');

			cat_text_in_text(buffer,text_matrix_nums[i][j]);

            add_nch_in_text(buffer,dif/2 + 1,' ');
		}

		cat_str_in_text(buffer,"|\r\n");

        cat_text_in_edit(edit, &buffer);
	}

	if(denominator != NULL)
	{
        init_text(buffer);

		cat_str_in_text(buffer,space);

	    //line_len = 2;
        add_nch_in_text(buffer,max_line_len, '-');
		cat_str_in_text(buffer,"\r\n");

		cat_str_in_text(buffer,space);

		short_text_t text_denominator = {0};
		print_num_in_text(text_denominator,(*denominator));

		num_len = get_strlen_in_edit(text_denominator.str);
		dif = max_line_len - num_len;

        add_nch_in_text(buffer,dif/2 + dif%2, ' ');

		cat_text_in_text(buffer,text_denominator);

        add_nch_in_text(buffer, dif/2, ' ')

        cat_text_in_edit(edit, &buffer);
	}
}
//print matrix in cramer

//show
void show_process_cramer(hedit_t edit, matrix_t matrix)
{
    text_t text = {};
	num_t sys_det;
	num_t solution[NXN_MAXN];

	matrix_t matrices[3];
	const char names[][6] = {"x = ","y = ","z = "};//names of each matrix
	pthread_t get_det_threads[3];//threads that will be used to get the det of each matrix
	get_det_args get_det_args[3];//args for the threads

    init_edit(edit);

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

	print_matrix_in_edit_cramer(edit,"det Ms = ","               ",NULL,matrix);

	cat_str_in_edit(edit,"\r\n");
	sys_det = get_sysdet("det Ms = ",matrix,edit);
	cat_str_in_edit(edit,"\r\n\r\n\r\n\r\n");

	if(get_num(sys_det) == 0)
	{
		cat_str_in_edit(edit,"\r\nNo se puede resolver el sistema pues la determinante del sistema es cero.");
        return;
	}

	//init matrices
	for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
	{
		matrices[cur_col] = matrix;

		//swap columns
		for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
		{
			num_t aux = matrices[cur_col].matrix[cur_row][cur_col];

			matrices[cur_col].matrix[cur_row][cur_col] = matrices[cur_col].matrix[cur_row][matrices[cur_col].ncolumns];

			matrices[cur_col].matrix[cur_row][matrices[cur_col].ncolumns] = aux;
		}

		//create threads
		get_det_args[cur_col].denominator = &sys_det;
		get_det_args[cur_col].matrix = &matrices[cur_col];
		get_det_args[cur_col].name = names[cur_col];
		pthread_create(&get_det_threads[cur_col], NULL, get_det_thread_routine, &get_det_args[cur_col]);
	}

	for(size_t i = 0; i < matrix.ncolumns; i++)
	{
		//print matrix from the det is going to be got/gotten
		print_matrix_in_edit_cramer(edit,names[i],"      ",&sys_det,matrix);
		cat_str_in_edit(edit,"\r\n\r\n");

		get_det_return_struct * return_struct;
		pthread_join(get_det_threads[i],(void**)&return_struct);//waits for that det
		cat_text_in_edit(edit,(const text_t*)&return_struct->process);//show process to get det
		solution[i] = simplify_num(divide_num(return_struct->det, sys_det));//get solution for that column
		free(return_struct);

		//show solution for that column
		init_text(text);
		cat_str_in_text(text," = ");
		print_num_in_text(text,solution[i]);
		cat_str_in_text(text,"\r\n\r\n\r\n\r\n");
        cat_text_in_edit(edit, &text);
	}

	//show the solution
    init_text(text);
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
    cat_text_in_edit(edit, &text);
}

//CRAMER------------------------------------------


//GAUSS-JORDAN------------------------------------

//print matrix in gauss/gauss-jordan
void print_matrix_in_edit_gauss_jordan(hedit_t hedit, matrix_t matrix)
{
    text_t buffer;
	short_text_t text_matrix_nums[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS] = {};
	size_t matrix_nums_len[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
	size_t matrix_nums_col_max_num[MATRIX_MAXNCOLUMNS];
	size_t max_num_len = 0, cur_num_len;
	num_t cur_num;
	size_t len_dif;

    //get "length in edit" of each number stored in matrix
    //also, it stores each number as s str in text_matrix_num
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

    //get the "maxlen in edit number" of each column
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

    //prints the matrix
	for (size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
	{
        init_text(buffer);

		add_ch_in_text(buffer,'|');
		for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
		{
			if(cur_col == matrix.ncolumns-1)
			{
				add_ch_in_text(buffer,'|');
			}

			cur_num_len = matrix_nums_len[cur_row][cur_col];

			len_dif = matrix_nums_col_max_num[cur_col] - cur_num_len;

            add_nch_in_text(buffer, len_dif/2 + len_dif%2, ' ');

			cat_text_in_text(buffer, text_matrix_nums[cur_row][cur_col]);

            add_nch_in_text(buffer, len_dif/2, ' ');
		}
		cat_str_in_text(buffer,"|\r\n");

        cat_text_in_edit(hedit, &buffer);
	}
}
//print matrix in gauss/gauss-jordan

//show gauss
void show_process_gauss(hedit_t edit, matrix_t matrix)
{
    init_edit(edit);
}
//show gauss

//show gauss-jordan
void show_process_gauss_jordan(hedit_t edit, matrix_t matrix)
{
    init_edit(edit);
}
//show gauss-jordan

//GAUSS-JORDAN------------------------------------

