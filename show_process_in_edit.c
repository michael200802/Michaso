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
#define cat_text_in_edit(edit, text)\
	cat_str_in_edit(edit, text.str);

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

//edit----------------------------------------------------

//print something-----------------------------------
typedef struct
{
	short_text_t _printed_nums[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
	short_text_t * printed_nums[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
	size_t nrows;
	size_t ncols;
	size_t longest_num_per_col[MATRIX_MAXNCOLUMNS];
	size_t nums_len[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
}print_matrix_args;

void get_print_matrix_args(const matrix_t * matrix, print_matrix_args * args)
{
	args->nrows = matrix->nrows;
	args->ncols = matrix->ncolumns;
	for(size_t cur_col = 0; cur_col < matrix->ncolumns; cur_col++)
	{
		args->longest_num_per_col[cur_col] = 0;
		for(size_t cur_row = 0; cur_row < matrix->nrows; cur_row++)
		{
			args->printed_nums[cur_row][cur_col] = &args->_printed_nums[cur_row][cur_col];

			init_text((*args->printed_nums[cur_row][cur_col]));

			print_num_in_text((*args->printed_nums[cur_row][cur_col]),matrix->matrix[cur_row][cur_col]);

			args->nums_len[cur_row][cur_col] = get_strlen_in_edit(args->printed_nums[cur_row][cur_col]->str);
			if(args->nums_len[cur_row][cur_col] > args->longest_num_per_col[cur_col])
			{
				args->longest_num_per_col[cur_col] = args->nums_len[cur_row][cur_col];
			}
		}
	}

}

#define show_str_centered(text, str, space, len)\
	{\
		size_t dif = space-len;\
		add_nch_in_text(text,dif/2+dif%2,' ');\
		cat_str_in_text(text,str);\
		add_nch_in_text(text,dif/2,' ');\
	}

#define print_matrix(text, args, before_printing_col, before_printing_num, after_printing_num, after_printing_col)\
	{\
		for(size_t cur_row = 0; cur_row < args.nrows; cur_row++)\
		{\
			before_printing_col;\
			for(size_t cur_col = 0; cur_col < args.ncols; cur_col++)\
			{\
				before_printing_num;\
				show_str_centered(text, args.printed_nums[cur_row][cur_col]->str, args.longest_num_per_col[cur_col], args.nums_len[cur_row][cur_col]);\
				after_printing_num;\
			}\
			after_printing_col\
		}\
	}

//print something-----------------------------------

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

			size_t dif = (line_len - denominator_len);

			add_nch_in_text((*process),name_len + (dif/2 + dif%2), ' ');
			print_num_in_text((*process),(*denominator));

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

			size_t dif = (line_len - denominator_len);

			add_nch_in_text((*process), name_len + (dif/2 + dif%2), ' ');
			print_num_in_text((*process),(*denominator));

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
num_t get_sysdet(const char * name, const matrix_t * matrix, hedit_t edit)
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
						add_ch_in_text(buffer,(matrix->is_3x3 ? '(':' '));
						for(size_t j = 0, jmax = (matrix->is_3x3? 3:2); j < jmax; j++, i++)
						{
							print_num_in_text(buffer,nums[i]);
							if(j+1 != jmax)
							{
								cat_str_in_text(buffer," * ");
							}
						}
						add_ch_in_text(buffer,(matrix->is_3x3 ? ')':' '));

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

		cat_text_in_edit(edit, buffer);
	}

	if(matrix->is_3x3)
	{
        init_text(buffer);

		cat_str_in_text(buffer,name);

        print_num_in_text(buffer,nums_sum[0]);
        cat_str_in_text(buffer," - ");
        print_num_in_text(buffer,nums_sum[1]);
        cat_str_in_text(buffer,"\r\n");

		cat_str_in_text(buffer,"\r\n");

        cat_text_in_edit(edit, buffer);
	}

    init_text(buffer);
	cat_str_in_text(buffer,name);
	print_num_in_text(buffer,det);
    cat_text_in_edit(edit, buffer);

	return det;
}
//get det and show process in edit

//show
void show_process_cramer(hedit_t edit, matrix_t matrix)
{
    text_t text = {};
	num_t sys_det;
	short_text_t text_sys_det = {};
	size_t text_sys_det_width;
	num_t solution[NXN_MAXN];

	matrix_t matrices[3];//matrices for each variable (x, y, z)
	print_matrix_args printm_args = {};//args for print a matrix
	const char operations_name[][6] = {"x = ","y = ","z = "};//names of each matrix
	const char matrices_name[][6] = {"x = |","y = |","z = |"};//names of each matrix
	pthread_t get_det_threads[3];//threads that will be used to get the det of each matrix
	get_det_args get_det_args[3];//args for the threads

    init_edit(edit);

	for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
	{
		bool all_zero = true, last_zero = matrix.matrix[cur_row][matrix.ncolumns-1].numerator == 0;
		for(size_t cur_col = 0, maxcol = matrix.ncolumns-1; cur_col < maxcol && all_zero; cur_col++)
		{
			all_zero = (matrix.matrix[cur_row][cur_col].numerator == 0);
		}
		if(all_zero)
		{
			if(last_zero)
			{
				cat_str_in_text(text, "Sistema compatible indeterminado: Una de las ecuaciones es equivalente a 0 = 0.");
			}
			else
			{
				cat_str_in_text(text, "Sistema incompatible: Una de las ecuaciones es equivalente a 0 = ");
				print_num_in_text(text,matrix.matrix[cur_row][matrix.ncolumns-1]);
				add_ch_in_text(text,'.');
			}
			cat_text_in_edit(edit,text);
			return;
		}
	}

	//init matrix and get print_matrix_args
	if(matrix.is_3x3)
	{
		for(size_t i = 0; i < 2; i++)
		{
			for(size_t j = 0; j < matrix.ncolumns; j++)
			{
				matrix.matrix[i+3][j] = matrix.matrix[i][j];
			}
		}

		matrix.ncolumns = 4;
		matrix.nrows = 5;

		get_print_matrix_args(&matrix,&printm_args);

		matrix.ncolumns = 3;
	}
	else
	{
		matrix.ncolumns = 3;
		matrix.nrows = 2;

		get_print_matrix_args(&matrix,&printm_args);

		matrix.ncolumns = 2;
	}
	printm_args.ncols--;

	print_matrix(text,printm_args,
		{
			if(cur_row == 1)
			{
				cat_str_in_text(text,"det Ms = |");
			}
			else
			{
				cat_str_in_text(text,"               |");
			}
		},
		{
			add_ch_in_text(text,' ');
		},
		{
			add_ch_in_text(text,' ');
		},
		{
			cat_str_in_text(text,"|\r\n");
		}
		);
	cat_text_in_edit(edit,text);

	cat_str_in_edit(edit,"\r\n");
	sys_det = get_sysdet("det Ms = ",&matrix,edit);
	cat_str_in_edit(edit,"\r\n\r\n\r\n\r\n");

	print_num_in_text(text_sys_det,sys_det);
	text_sys_det_width = get_strlen_in_edit(text_sys_det.str);

	if(sys_det.numerator == 0)
	{
		cat_str_in_edit(edit,"\r\nSistema incompatible: no se puede resolver el sistema pues la determinante del sistema es cero.");
        return;
	}

	//init matrices
	for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
	{
		matrices[cur_col] = matrix;

		//swap columns
		for(size_t cur_row = 0; cur_row < matrix.nrows; cur_row++)
		{
			{
				num_t aux = matrices[cur_col].matrix[cur_row][cur_col];

				matrices[cur_col].matrix[cur_row][cur_col] = matrices[cur_col].matrix[cur_row][matrices[cur_col].ncolumns];

				matrices[cur_col].matrix[cur_row][matrices[cur_col].ncolumns] = aux;
			}
		}

		//create threads
		get_det_args[cur_col].denominator = &sys_det;
		get_det_args[cur_col].matrix = &matrices[cur_col];
		get_det_args[cur_col].name = operations_name[cur_col];
		pthread_create(&get_det_threads[cur_col], NULL, get_det_thread_routine, &get_det_args[cur_col]);
	}

	for(size_t i = 0; i < matrix.ncolumns; i++)
	{
		//print matrix from the det is going to be got/gotten

		for(size_t cur_row = 0, cur_col = i, last_column = printm_args.ncols; cur_row < matrix.nrows; cur_row++)
		{
			{
				short_text_t * aux = printm_args.printed_nums[cur_row][cur_col];
				printm_args.printed_nums[cur_row][cur_col] = printm_args.printed_nums[cur_row][last_column];
				printm_args.printed_nums[cur_row][last_column] = aux;
			}
			{
				size_t aux = printm_args.nums_len[cur_row][cur_col];
				printm_args.nums_len[cur_row][cur_col] = printm_args.nums_len[cur_row][last_column];
				printm_args.nums_len[cur_row][last_column] = aux;
			}
		}
		{
			size_t aux = printm_args.longest_num_per_col[i];
			printm_args.longest_num_per_col[i] = printm_args.longest_num_per_col[printm_args.ncols];
			printm_args.longest_num_per_col[printm_args.ncols] = aux;
		}

		init_text(text);
		print_matrix(text,printm_args,
			{
				if(cur_row == 1)
				{
					cat_str_in_text(text,matrices_name[i]);
				}
				else
				{
					cat_str_in_text(text,"      |");
				}
			},
			{
				add_ch_in_text(text,' ');
			},
			{
				add_ch_in_text(text,' ');
			},
			{
				cat_str_in_text(text,"|\r\n");
			}
			);

		size_t matrix_width = 2, dif;
		for(size_t i = 0; i < printm_args.ncols; i++)
		{
			matrix_width += printm_args.longest_num_per_col[i]+2;
		}
		dif = matrix_width - text_sys_det_width;
		cat_str_in_text(text,"      ");
		add_nch_in_text(text,matrix_width,'-');
		cat_str_in_text(text,"\r\n");
		cat_str_in_text(text,"      ");
		add_nch_in_text(text, dif/2 + dif%2, ' ');
		cat_text_in_text(text,text_sys_det);
		cat_str_in_text(text,"\r\n\r\n");
		
		cat_text_in_edit(edit,text);

		for(size_t cur_row = 0, cur_col = i, last_column = printm_args.ncols; cur_row < matrix.nrows; cur_row++)
		{
			{
				short_text_t * aux = printm_args.printed_nums[cur_row][cur_col];
				printm_args.printed_nums[cur_row][cur_col] = printm_args.printed_nums[cur_row][last_column];
				printm_args.printed_nums[cur_row][last_column] = aux;
			}
			{
				size_t aux = printm_args.nums_len[cur_row][cur_col];
				printm_args.nums_len[cur_row][cur_col] = printm_args.nums_len[cur_row][last_column];
				printm_args.nums_len[cur_row][last_column] = aux;
			}
		}
		{
			size_t aux = printm_args.longest_num_per_col[i];
			printm_args.longest_num_per_col[i] = printm_args.longest_num_per_col[printm_args.ncols];
			printm_args.longest_num_per_col[printm_args.ncols] = aux;
		}

		get_det_return_struct * return_struct;
		pthread_join(get_det_threads[i],(void**)&return_struct);//waits for that det
		cat_text_in_edit(edit,return_struct->process);//show process to get det
		solution[i] = simplify_num(divide_num(return_struct->det, sys_det));//get solution for that column
		free(return_struct);

		//show solution for that column
		init_text(text);
		cat_str_in_text(text," = ");
		print_num_in_text(text,solution[i]);
		cat_str_in_text(text,"\r\n\r\n\r\n\r\n");
        cat_text_in_edit(edit, text);
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
    cat_text_in_edit(edit, text);
}

//CRAMER------------------------------------------

//GAUSS-JORDAN------------------------------------

static inline void get_common_product(num_t num1, num_t num2, num_t * prod1, num_t * prod2)
{
	num_t quotient = divide_num(num1, num2);
	set_num((*prod1),quotient.denominator,1,NUM_STATE_INT);
	set_num((*prod2),quotient.numerator,1,NUM_STATE_INT);
	*prod1 = simplify_num(*prod1);
	*prod2 = simplify_num(*prod2);
}

typedef enum {SUCCESS,FAILURE,ALREADY_0} make_it_zero_return;
static inline make_it_zero_return make_it_zero(hedit_t edit, matrix_t * matrix, size_t row, size_t col)
{
	size_t chosen_row;//chosen row for the rest
	num_t prod1, prod2;//numbers that will be used to multiply the rows
	short_text_t text_prod1 = {}, text_prod2 = {};//prod1 and prod2 in text
	size_t prod1_len_in_edit, prod2_len_in_edit, prods_col_len_in_edit;//lens of prod1 and prod2
	short_text_t text_rest = {};//text that will store "F1*x-F2*y"
	size_t text_rest_len_in_edit;//text_rest len in edit
	matrix_t rest;//the matrix that will store the rest between rows
	print_matrix_args printm_args;//print rest
	text_t buffer = {};

	if(matrix->matrix[row][col].numerator == 0)
	{
		return ALREADY_0;
	}

	chosen_row = row;
	if(matrix->is_3x3)
	{
		for(char cur_row = matrix->nrows-1; cur_row != -1; cur_row--)
		{
			if(matrix->matrix[cur_row][col].numerator != 0 && row != cur_row)
			{
				chosen_row = cur_row;
				break;
			}
		}
	}
	else
	{
		switch (row)
		{
		case 0:
			if(matrix->matrix[1][col].numerator != 0)
			{
				chosen_row = 1;
			}
			break;
		case 1:
			if(matrix->matrix[0][col].numerator != 0)
			{
				chosen_row = 0;
			}
			break;
		}
	}
	if(chosen_row == row)
	{
		cat_str_in_edit(edit, "Sistema incompatible.");
		return FAILURE;
	}

	get_common_product(matrix->matrix[row][col],matrix->matrix[chosen_row][col],&prod1,&prod2);
	prod2.numerator *= -1;

	print_in_text(text_prod1,"F%zux",row+1);
	print_num_in_text(text_prod1,prod1);

	print_in_text(text_prod2,"F%zux",chosen_row+1);
	print_num_in_text(text_prod2,prod2);

	cat_text_in_text(text_rest,text_prod1);
	cat_str_in_text(text_rest," + ");
	cat_text_in_text(text_rest,text_prod2);

	prod1_len_in_edit = get_strlen_in_edit(text_prod1.str);
	prod2_len_in_edit = get_strlen_in_edit(text_prod2.str);
	text_rest_len_in_edit = get_strlen_in_edit(text_rest.str);

	prods_col_len_in_edit = text_rest_len_in_edit+2;

	rest.nrows = 3;
	rest.ncolumns = matrix->ncolumns;
	rest.is_3x3 = true;
	for(size_t cur_col = 0; cur_col < rest.ncolumns; cur_col++)
	{
		rest.matrix[0][cur_col] = multiply_num(matrix->matrix[row][cur_col],prod1);
		rest.matrix[1][cur_col] = multiply_num(matrix->matrix[chosen_row][cur_col],prod2);
		rest.matrix[2][cur_col] = sum_num(rest.matrix[0][cur_col],rest.matrix[1][cur_col]);
	}

	get_print_matrix_args(&rest,&printm_args);

	print_matrix(buffer, printm_args,
		{
			switch (cur_row)
			{
			case 0:
				show_str_centered(buffer,text_prod1.str,prods_col_len_in_edit,prod1_len_in_edit);
				break;
			case 1:
				show_str_centered(buffer,text_prod2.str,prods_col_len_in_edit,prod2_len_in_edit);
				break;
			case 2:
				show_str_centered(buffer,text_rest.str,prods_col_len_in_edit,text_rest_len_in_edit);
				break;
			}
			cat_str_in_text(buffer,":    |");
		},
		{
			if(cur_col == printm_args.ncols-1)
			{
				cat_str_in_text(buffer," | ");
			}
			else
			{
				add_ch_in_text(buffer,' ');
			}
		},
		{
			add_ch_in_text(buffer,' ');
		},
		{
			cat_str_in_text(buffer,"|\r\n");
		}
	);

	cat_str_in_text(buffer,"\r\n\r\n");

	size_t nzero = 0;
	for(size_t cur_col = 0; cur_col < matrix->ncolumns; cur_col++)
	{
		matrix->matrix[row][cur_col] = rest.matrix[2][cur_col];
		if(rest.matrix[2][cur_col].numerator == 0)
		{
			nzero++;
		}
	}
	if(nzero == matrix->ncolumns)
	{
		cat_str_in_text(buffer,"Sistema compatible indeterminado.");
		cat_text_in_edit(edit,buffer);
		return FAILURE;
	}
	else if(nzero == matrix->ncolumns-1 && matrix->matrix[row][nzero].numerator != 0)
	{
		cat_str_in_text(buffer,"Sistema incompatible.");
		cat_text_in_edit(edit,buffer);			
		return FAILURE;
	}

	cat_text_in_edit(edit,buffer);

	return SUCCESS;
}

void solve_sys(hedit_t edit, matrix_t sys)
{
	text_t buffer = {};
	num_t solutions[NXN_MAXN];
	for(size_t i = 0; i < NXN_MAXN; i++)
	{
		set_num(solutions[i],0,0,NUM_STATE_INT);
	}
	for(char cur_row = sys.nrows-1; cur_row != -1; cur_row--)
	{
		num_t total;
		set_num(total,0,0,NUM_STATE_INT);
		for(char cur_col = cur_row+1, max_col = sys.ncolumns-1; cur_col < max_col; cur_col++)
		{
			total = sum_num(total,multiply_num(sys.matrix[cur_row][cur_col],solutions[cur_col]));
			sys.matrix[cur_row][cur_col].numerator = 0;
		}

		char cur_col = cur_row;
		sys.matrix[cur_row][sys.ncolumns-1] = rest_num(sys.matrix[cur_row][sys.ncolumns-1],total);
		sys.matrix[cur_row][sys.ncolumns-1] = divide_num(sys.matrix[cur_row][sys.ncolumns-1],sys.matrix[cur_row][cur_col]);
		set_num(sys.matrix[cur_row][cur_col],1,1,NUM_STATE_INT);
		solutions[cur_col] = sys.matrix[cur_row][sys.ncolumns-1];

		cat_str_in_text(buffer,"\r\n");
		matrix_to_system(sys,&buffer);
		cat_str_in_text(buffer,"\r\n");
	}
	cat_str_in_text(buffer,"Solucion: (");
	for(size_t i = 0, maxi = sys.ncolumns-1; i < maxi; i++)
	{
		print_num_in_text(buffer,solutions[i]);
		if(i+1 != maxi)
		{
			cat_str_in_text(buffer," ; ");
		}
	}
	cat_str_in_text(buffer,")\r\n");
	cat_text_in_edit(edit,buffer);
}

//show gauss and returns the final matrix
matrix_t * show_process_gauss(hedit_t edit, matrix_t matrix)
{
	static matrix_t matrix_result;
	matrix_result = matrix;
	print_matrix_args printm_args;
	text_t buffer = {};

    init_edit(edit);

	get_print_matrix_args(&matrix_result,&printm_args);
	print_matrix(buffer,printm_args,
		{
			add_ch_in_text(buffer,'|');
		},
		{
			if(cur_col == printm_args.ncols-1)
			{
				cat_str_in_text(buffer," | ");
			}
			else
			{
				add_ch_in_text(buffer,' ');
			}
		},
		{
			add_ch_in_text(buffer,' ');
		},
		{
			cat_str_in_text(buffer,"|\r\n");
		}
		);
	cat_str_in_text(buffer,"\r\n");

	bool indet_sys = false;
	for(size_t cur_row = 0; cur_row < matrix_result.nrows; cur_row++)
	{
		bool all_zero = true, last_elem_zero = (matrix_result.matrix[cur_row][matrix_result.ncolumns-1].numerator == 0);
		for(size_t cur_col = 0, max_col = matrix_result.ncolumns-1; cur_col < max_col && all_zero; cur_col++)
		{
			all_zero = (matrix_result.matrix[cur_row][cur_col].numerator == 0);
		}
		if(all_zero)
		{
			if(last_elem_zero)
			{
				indet_sys = true;
			}
			else
			{
				cat_str_in_text(buffer, "\r\n\r\nSistema incompatible.");
				cat_text_in_edit(edit,buffer);
				return NULL;
			}
		}
	}
	if(indet_sys)
	{
		cat_str_in_text(buffer, "\r\n\r\nSistema compatible indeterminado.");
		cat_text_in_edit(edit,buffer);
		return NULL;
	}

	cat_text_in_edit(edit,buffer);

	switch (make_it_zero(edit,&matrix_result,1,0))
	{
		case SUCCESS:
			for(size_t cur_col = 0; cur_col < matrix_result.ncolumns; cur_col++)
			{
				init_text((*printm_args.printed_nums[1][cur_col]));
				print_num_in_text((*printm_args.printed_nums[1][cur_col]),matrix_result.matrix[1][cur_col]);
				printm_args.nums_len[1][cur_col] = get_strlen_in_edit(printm_args.printed_nums[1][cur_col]->str);
				if(printm_args.nums_len[1][cur_col]  > printm_args.longest_num_per_col[cur_col])
				{
					printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[1][cur_col];
				}
			}
			init_text(buffer);
			print_matrix(buffer,printm_args,
				{
					add_ch_in_text(buffer,'|');
				},
				{
					if(cur_col == printm_args.ncols-1)
					{
						cat_str_in_text(buffer," | ");
					}
					else
					{
						add_ch_in_text(buffer,' ');
					}
				},
				{
					add_ch_in_text(buffer,' ');
				},
				{
					cat_str_in_text(buffer,"|\r\n");
				}
				);
			cat_str_in_text(buffer,"\r\n");
			cat_text_in_edit(edit,buffer);
			break;
		case FAILURE:
			return NULL;
			break;
		case ALREADY_0:
			break;
	}

	if(matrix.is_3x3)
	{
		switch (make_it_zero(edit,&matrix_result,2,0))
		{
			case SUCCESS:
				for(size_t cur_col = 0; cur_col < matrix_result.ncolumns; cur_col++)
				{
					init_text((*printm_args.printed_nums[2][cur_col]));
					print_num_in_text((*printm_args.printed_nums[2][cur_col]),matrix_result.matrix[2][cur_col]);
					printm_args.nums_len[2][cur_col] = get_strlen_in_edit(printm_args.printed_nums[2][cur_col]->str);
					if(printm_args.nums_len[2][cur_col]  > printm_args.longest_num_per_col[cur_col])
					{
						printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[2][cur_col];
					}
				}	
				init_text(buffer);
				print_matrix(buffer,printm_args,
					{
						add_ch_in_text(buffer,'|');
					},
					{
						if(cur_col == printm_args.ncols-1)
						{
							cat_str_in_text(buffer," | ");
						}
						else
						{
							add_ch_in_text(buffer,' ');
						}
					},
					{
						add_ch_in_text(buffer,' ');
					},
					{
						cat_str_in_text(buffer,"|\r\n");
					}
					);
				cat_str_in_text(buffer,"\r\n");
				cat_text_in_edit(edit,buffer);
				break;
			case FAILURE:
				return NULL;
				break;
			case ALREADY_0:
				break;
		}

		switch (make_it_zero(edit,&matrix_result,2,1))
		{
			case SUCCESS:
				for(size_t cur_col = 0; cur_col < matrix_result.ncolumns; cur_col++)
				{
					init_text((*printm_args.printed_nums[2][cur_col]));
					print_num_in_text((*printm_args.printed_nums[2][cur_col]),matrix_result.matrix[2][cur_col]);
					printm_args.nums_len[2][cur_col] = get_strlen_in_edit(printm_args.printed_nums[2][cur_col]->str);
					if(printm_args.nums_len[2][cur_col]  > printm_args.longest_num_per_col[cur_col])
					{
						printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[2][cur_col];
					}
				}	
				init_text(buffer);
				print_matrix(buffer,printm_args,
					{
						add_ch_in_text(buffer,'|');
					},
					{
						if(cur_col == printm_args.ncols-1)
						{
							cat_str_in_text(buffer," | ");
						}
						else
						{
							add_ch_in_text(buffer,' ');
						}
					},
					{
						add_ch_in_text(buffer,' ');
					},
					{
						cat_str_in_text(buffer,"|\r\n");
					}
					);
				cat_str_in_text(buffer,"\r\n");
				cat_text_in_edit(edit,buffer);
				break;
			case FAILURE:
				return NULL;
				break;
			case ALREADY_0:
				break;
		}

	}

	init_text(buffer);
	matrix_to_system(matrix_result,&buffer);
	cat_text_in_edit(edit,buffer);
	solve_sys(edit,matrix_result);

	return &matrix_result;
}
//show gauss and returns the final matrix

//show gauss-jordan
void show_process_gauss_jordan(hedit_t edit, matrix_t matrix)
{
	matrix_t * jordan_matrix;
	print_matrix_args printm_args;
	text_t buffer = {};

    init_edit(edit);

	jordan_matrix = show_process_gauss(edit, matrix);
	if(jordan_matrix == NULL)
	{
		return;
	}
	matrix = *jordan_matrix;

	cat_str_in_text(buffer,"\r\n");
	get_print_matrix_args(jordan_matrix,&printm_args);
	print_matrix(buffer,printm_args,
		{
			add_ch_in_text(buffer,'|');
		},
		{
			if(cur_col == printm_args.ncols-1)
			{
				cat_str_in_text(buffer," | ");
			}
			else
			{
				add_ch_in_text(buffer,' ');
			}
		},
		{
			add_ch_in_text(buffer,' ');
		},
		{
			cat_str_in_text(buffer,"|\r\n");
		}
		);
	cat_str_in_text(buffer,"\r\n");
	
	cat_text_in_edit(edit,buffer);

	if(matrix.is_3x3)
	{
		switch (make_it_zero(edit,&matrix,1,2))
		{
			case SUCCESS:
				for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
				{
					init_text((*printm_args.printed_nums[1][cur_col]));
					print_num_in_text((*printm_args.printed_nums[1][cur_col]),matrix.matrix[1][cur_col]);
					printm_args.nums_len[1][cur_col] = get_strlen_in_edit(printm_args.printed_nums[1][cur_col]->str);
					if(printm_args.nums_len[1][cur_col]  > printm_args.longest_num_per_col[cur_col])
					{
						printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[1][cur_col];
					}
				}
				init_text(buffer);
				print_matrix(buffer,printm_args,
					{
						add_ch_in_text(buffer,'|');
					},
					{
						if(cur_col == printm_args.ncols-1)
						{
							cat_str_in_text(buffer," | ");
						}
						else
						{
							add_ch_in_text(buffer,' ');
						}
					},
					{
						add_ch_in_text(buffer,' ');
					},
					{
						cat_str_in_text(buffer,"|\r\n");
					}
					);
				cat_str_in_text(buffer,"\r\n");
				cat_text_in_edit(edit,buffer);
				break;
			case FAILURE:
				return;
			case ALREADY_0:
				break;
		}

		switch (make_it_zero(edit,&matrix,0,2))
		{
			case SUCCESS:
				for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
				{
					init_text((*printm_args.printed_nums[0][cur_col]));
					print_num_in_text((*printm_args.printed_nums[0][cur_col]),matrix.matrix[0][cur_col]);
					printm_args.nums_len[0][cur_col] = get_strlen_in_edit(printm_args.printed_nums[0][cur_col]->str);
					if(printm_args.nums_len[0][cur_col]  > printm_args.longest_num_per_col[cur_col])
					{
						printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[0][cur_col];
					}
				}	
				init_text(buffer);
				print_matrix(buffer,printm_args,
					{
						add_ch_in_text(buffer,'|');
					},
					{
						if(cur_col == printm_args.ncols-1)
						{
							cat_str_in_text(buffer," | ");
						}
						else
						{
							add_ch_in_text(buffer,' ');
						}
					},
					{
						add_ch_in_text(buffer,' ');
					},
					{
						cat_str_in_text(buffer,"|\r\n");
					}
					);
				cat_str_in_text(buffer,"\r\n");
				cat_text_in_edit(edit,buffer);
				break;
			case FAILURE:
				return;
			case ALREADY_0:
				break;
		}

	}

	switch (make_it_zero(edit,&matrix,0,1))
	{
		case SUCCESS:
			for(size_t cur_col = 0; cur_col < matrix.ncolumns; cur_col++)
			{
				init_text((*printm_args.printed_nums[0][cur_col]));
				print_num_in_text((*printm_args.printed_nums[0][cur_col]),matrix.matrix[0][cur_col]);
				printm_args.nums_len[0][cur_col] = get_strlen_in_edit(printm_args.printed_nums[0][cur_col]->str);
				if(printm_args.nums_len[0][cur_col]  > printm_args.longest_num_per_col[cur_col])
				{
					printm_args.longest_num_per_col[cur_col] = printm_args.nums_len[0][cur_col];
				}
			}	
			init_text(buffer);
			print_matrix(buffer,printm_args,
				{
					add_ch_in_text(buffer,'|');
				},
				{
					if(cur_col == printm_args.ncols-1)
					{
						cat_str_in_text(buffer," | ");
					}
					else
					{
						add_ch_in_text(buffer,' ');
					}
				},
				{
					add_ch_in_text(buffer,' ');
				},
				{
					cat_str_in_text(buffer,"|\r\n");
				}
				);
			cat_str_in_text(buffer,"\r\n");
			cat_text_in_edit(edit,buffer);
			break;
		case FAILURE:
			return;
		case ALREADY_0:
			break;
	}

	init_text(buffer);
	matrix_to_system(matrix,&buffer);
	cat_text_in_edit(edit,buffer);
	solve_sys(edit,matrix);
}
//show gauss-jordan

//GAUSS-JORDAN------------------------------------

