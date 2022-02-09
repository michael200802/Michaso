#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <ctype.h>
#include <pthread.h>
#include "num.h"
#include "text.h"
#include "resolve_matrix.h"
#include <limits.h>

#define NUMBER_MAX_LEN 8

#define LABEL_MATRIX_ELEM_ID 12
#define COMBOBOX_NXN_ID 13
#define COMBOBOX_METODO_ID 14
#define COMBOBOX_NXN_LABEL_ID 15
#define COMBOBOX_METODO_LABEL_ID 16
#define EDIT_PROCESS_ID 17
#define LABEL_EDIT_EQUATION_SYSTEM_ID 18
#define EDIT_EQUATION_SYSTEM_ID 19

#define LINE_HEIGHT 20

#define LABEL_DISTANCE LINE_HEIGHT

#define MATRIX_X 10
#define MATRIX_Y 150

#define ROW_SIZE LINE_HEIGHT
#define COLUMN_SIZE 60

#define METHOD_CRAMER "Metodo de Cramer."
#define METHOD_G "Metodo de Gauss. "
#define METHOD_G_J "Metodo de Gauss-Jordan."

BOOL get_matrix(HWND (*matrix_edits)[NXN_MAXN+1], BOOL is_3x3, matrix_t * matrix)
{
	matrix->is_3x3 = is_3x3;
	matrix->nrows = is_3x3? NXN_MAXN : NXN_MAXN-1;
	matrix->ncolumns = is_3x3? NXN_MAXN+1 : NXN_MAXN;
	for(size_t i = 0; i < matrix->nrows; i++)
	{
		for(size_t j = 0; j < matrix->ncolumns; j++)
		{
			char buffer[NUMBER_MAX_LEN];
			Edit_GetText(matrix_edits[i][j],buffer,NUMBER_MAX_LEN);
			if(str_to_num(buffer,&matrix->matrix[i][j]) == false)
			{
				return false;
			}
			printnum(matrix->matrix[i][j]);
		}
		putchar('\n');
	}
	return TRUE;
}

text_t get_process(matrix_t matrix, HWND CB_method)
{
	char method_str[30];
	ComboBox_GetText(CB_method,method_str,30);

	if(strcmp(method_str,METHOD_CRAMER) == 0)
	{
		puts(METHOD_CRAMER);
		return cramer(matrix);
	}
	else if(strcmp(method_str,METHOD_G) == 0)
	{
		puts(METHOD_G);
		return gauss(matrix);
	}
	else if(strcmp(method_str,METHOD_G_J) == 0)
	{
		puts(METHOD_G_J);
		text_t process_gauss = gauss(matrix), process_jordan = jordan(matrix);
		cat_text_in_text(process_gauss,process_jordan);
		return process_gauss;
	}
	text_t defprocess = {};
	cat_str_in_text(defprocess,"No se ha seleccionado ningun metodo.");
	return defprocess;
}

struct
{
	pthread_mutex_t matrix_has_changed_mutex;
	bool matrix_has_changed;
	HWND (*matrix_elem)[NXN_MAXN+1];
	bool is_3x3;
	HWND edit_equation_system;
	HWND combobox_method;
	HWND edit_process;
	pthread_mutex_t sleep_mutex;
	pthread_mutex_t sleep_condv;
}show_thread_args;

#define send_signal_to_show_thread()						\
	pthread_mutex_lock(&show_thread_args.matrix_has_changed_mutex);		\
	show_thread_args.matrix_has_changed = true;				\
	pthread_mutex_unlock(&show_thread_args.matrix_has_changed_mutex);	\
	pthread_cond_signal(&show_thread_args.sleep_condv);

void * show_thread_routine(void * arg)
{
	while(true)
	{
		pthread_mutex_lock(&show_thread_args.sleep_mutex);//sleep mutex

		pthread_mutex_lock(&show_thread_args.matrix_has_changed_mutex);//matrix_has_changed mutex
		bool is_matrix_changed = show_thread_args.matrix_has_changed;//is ready???
		show_thread_args.matrix_has_changed = false;
		pthread_mutex_unlock(&show_thread_args.matrix_has_changed_mutex);//matrix_has_changed mutex

		if(is_matrix_changed == false)
		{
			pthread_cond_wait(&show_thread_args.sleep_condv,&show_thread_args.sleep_mutex);
		}

		static int con = 0;
		printf("%d: edit messages\n",con++);

		matrix_t matrix;
		BOOL matrix_ready = get_matrix(show_thread_args.matrix_elem,show_thread_args.is_3x3,&matrix);
		text_t text_equation_system = {};//1
		text_t text_process = {};//2

		if(matrix_ready)
		{
			puts("ready");

			text_equation_system = matrix_to_system(matrix);//1
			text_process = get_process(matrix,show_thread_args.combobox_method);//2

			printf("Sistema de ecuaciones:\n%s",text_equation_system.str);
		}
		else
		{
			puts("non-ready");

			cat_str_in_text(text_equation_system,"Ingrese los datos en la matriz.");//1
			cat_str_in_text(text_process,"Ingrese los datos en la matriz.");//2
		}

		Edit_SetText(show_thread_args.edit_equation_system,text_equation_system.str);

		Edit_SetText(show_thread_args.edit_process,text_process.str);

		pthread_mutex_unlock(&show_thread_args.sleep_mutex);//sleep mutex

		destroy_text(text_equation_system);//1
		destroy_text(text_process);//2
	}
	return NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//handle to the program's instance
	static HINSTANCE hCurInstance;
	//handles for the windows/controls
	static HWND combo_box_nxn_label;
	static HWND combo_box_nxn;
	static HWND combo_box_metodo_label;
	static HWND combo_box_metodo;
	static HWND label_matrix_elem;
	static HWND matrix_elem[NXN_MAXN][NXN_MAXN+1];
	static HWND edit_process;
	static HWND edit_equation_system;
	static HWND label_edit_equation_system;

	switch(msg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case COMBOBOX_NXN_ID:
					//change size 3x3 --> 2x2 --> 3x3
					if(CBN_SELCHANGE == HIWORD(wParam))
					{
						char ch[2];
						ComboBox_GetText(combo_box_nxn,ch,2);
						switch(ch[0])
						{
							case '2'://2x2 --> 3x3
								if(show_thread_args.is_3x3 == TRUE)//the current nxn is 3x3
								{
									//remove the hole third row
									for(char i = 0; i < NXN_MAXN; i++)
									{
										DestroyWindow(matrix_elem[NXN_MAXN-1][i]);
									}
									//remove the column number 4
									for(char i = 0; i < NXN_MAXN; i++)
									{
										DestroyWindow(matrix_elem[i][NXN_MAXN]);
									}
									//change label
									DestroyWindow(label_matrix_elem);
									label_matrix_elem = CreateWindow
									(
										WC_STATIC,
										"       X               Y               TI",
										WS_VISIBLE|WS_CHILD|WS_BORDER|SS_LEFT,
										MATRIX_X,
										MATRIX_Y-20,
										COLUMN_SIZE*3,
										20,
										hWnd,
										(HMENU)LABEL_MATRIX_ELEM_ID,
										hCurInstance,
										NULL
									);
									show_thread_args.is_3x3 = FALSE;
								}
								break;
							case '3'://3x3 --> 2x2
								if(show_thread_args.is_3x3 == FALSE)//the current nxn is 2x2
								{
									//create new edits (2,0) (2,1) (2,2)
									for(char i = 0; i < NXN_MAXN; i++)
									{
										matrix_elem[NXN_MAXN-1][i] = CreateWindow
										(
											WC_EDIT,
											"",
											WS_CHILD|WS_BORDER|WS_VISIBLE|ES_CENTER,
											MATRIX_X+COLUMN_SIZE*i,
											MATRIX_Y+ROW_SIZE*(NXN_MAXN-1),
											COLUMN_SIZE,
											ROW_SIZE,
											hWnd,
											(HMENU)(NXN_MAXN*(NXN_MAXN-1)+i),
											hCurInstance,
											NULL
										);
									}
									//create new edits (0,2) (1,2) (2,2)
									for(char i = 0; i < NXN_MAXN; i++)
									{
										matrix_elem[i][NXN_MAXN] = CreateWindow
										(
											WC_EDIT,
											"",
											WS_CHILD|WS_BORDER|WS_VISIBLE|ES_CENTER,
											MATRIX_X+COLUMN_SIZE*NXN_MAXN,
											MATRIX_Y+ROW_SIZE*i,
											COLUMN_SIZE,
											ROW_SIZE,
											hWnd,
											(HMENU)(i*(NXN_MAXN+1)+3),
											hCurInstance,
											NULL
										);
									}
									DestroyWindow(label_matrix_elem);
									label_matrix_elem = CreateWindow
									(
										WC_STATIC,
										"       X               Y               Z               TI",
										WS_VISIBLE|WS_CHILD|WS_BORDER|SS_LEFT,
										MATRIX_X,
										MATRIX_Y-20,
										COLUMN_SIZE*4,
										20,
										hWnd,
										(HMENU)LABEL_MATRIX_ELEM_ID,
										hCurInstance,
										NULL
									);
									show_thread_args.is_3x3 = TRUE;
								}
								break;
						}
						send_signal_to_show_thread();
					}
					break;
				case COMBOBOX_METODO_ID:
					if(CBN_SELCHANGE == HIWORD(wParam))
					{
						send_signal_to_show_thread();
					}
					break;
				default:
					if(LOWORD(wParam) < (NXN_MAXN+1)*NXN_MAXN && HIWORD(wParam) == EN_CHANGE)
					{
						send_signal_to_show_thread();
					}
			}
			break;
		case WM_CREATE:
			//obtengo la instacia del programa
			hCurInstance = ((CREATESTRUCT*)lParam)->hInstance;

			//se crea el label para el COMBOBOX metodo
			combo_box_metodo_label = CreateWindow
			(
				WC_STATIC,
				"Metodo de resolucion:",
				WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|SS_LEFT|SS_SIMPLE,
				MATRIX_X,
				30-LABEL_DISTANCE,
				200,
				20,
				hWnd,
				(HMENU)COMBOBOX_METODO_LABEL_ID,
				hCurInstance,
				NULL
			);

			//se crea el COMBOBOX metodo
			combo_box_metodo = CreateWindow
			(
				WC_COMBOBOX,
				"",
				WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|CBS_DROPDOWNLIST|CBS_HASSTRINGS,
				MATRIX_X,
				30,
				190,
				100,
				hWnd,
				(HMENU)COMBOBOX_METODO_ID,
				hCurInstance,
				NULL
			);
			ComboBox_AddString(combo_box_metodo,METHOD_CRAMER);
			ComboBox_AddString(combo_box_metodo,METHOD_G);
			ComboBox_AddString(combo_box_metodo,METHOD_G_J);

			//se crea el label para el COMBOBOX nxn
			combo_box_nxn_label = CreateWindow
			(
				WC_STATIC,
				"Longitud de la matriz:",
				WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|SS_LEFT|SS_SIMPLE,
				MATRIX_X,
				90-LABEL_DISTANCE,
				200,
				20,
				hWnd,
				(HMENU)COMBOBOX_NXN_LABEL_ID,
				hCurInstance,
				NULL
			);

			//Se crea el COMBOBOX nxn 
			combo_box_nxn = CreateWindow
			(
				WC_COMBOBOX,
				"",
				WS_VISIBLE|WS_CHILD|WS_OVERLAPPED|CBS_DROPDOWNLIST|CBS_HASSTRINGS,
				MATRIX_X,
				90,
				100,
				100,
				hWnd,
				(HMENU)COMBOBOX_NXN_ID,
				hCurInstance,
				NULL
			);
			ComboBox_AddString(combo_box_nxn,"3x3");
			ComboBox_AddString(combo_box_nxn,"2x2");

			//se crea el label con el titulo de la matriz
			label_matrix_elem = CreateWindow
			(
				WC_STATIC,
				"       X               Y               Z               TI",
				WS_VISIBLE|WS_CHILD|WS_BORDER|SS_LEFT,
				MATRIX_X,
				MATRIX_Y-20,
				COLUMN_SIZE*4,
				20,
				hWnd,
				(HMENU)LABEL_MATRIX_ELEM_ID,
				hCurInstance,
				NULL
			);

			//Se crean los edits para la matriz
			for(long int i = 0; i < NXN_MAXN; i++)
			{
				for(long int j = 0; j < NXN_MAXN+1; j++)
				{
					matrix_elem[i][j] = CreateWindow
					(
						WC_EDIT,
						"1",
						WS_CHILD|WS_BORDER|WS_VISIBLE|ES_CENTER,
						MATRIX_X+COLUMN_SIZE*j,
						MATRIX_Y+ROW_SIZE*i,
						COLUMN_SIZE,
						ROW_SIZE,
						hWnd,
						(HMENU)(i*(NXN_MAXN+1) + j),
						hCurInstance,
						NULL
					);
				}
			}

			//label para el edit con el sistema de ecuaciones
			label_edit_equation_system = CreateWindow
			(
				WC_STATIC,
				"Sistema de ecuaciones",
				WS_CHILD|WS_VISIBLE|WS_BORDER|SS_LEFT,
				MATRIX_X,
				MATRIX_Y + ROW_SIZE*NXN_MAXN + LINE_HEIGHT*3 - LABEL_DISTANCE,
				COLUMN_SIZE*(NXN_MAXN+1),
				LINE_HEIGHT,
				hWnd,
				(HMENU)LABEL_EDIT_EQUATION_SYSTEM_ID,
				hCurInstance,
				NULL
			);

			//edit con el sistema de ecuaciones
			edit_equation_system = CreateWindow
			(
				WC_EDIT,
				"",
				WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|WS_HSCROLL|ES_READONLY|ES_LEFT|ES_MULTILINE,
				MATRIX_X,
				MATRIX_Y + ROW_SIZE*NXN_MAXN + LINE_HEIGHT*3,
				COLUMN_SIZE*(NXN_MAXN+1),
				LINE_HEIGHT*4,
				hWnd,
				(HMENU)EDIT_EQUATION_SYSTEM_ID,
				hCurInstance,
				NULL
			);

			//edit para la respuesta
			edit_process = CreateWindow
			(
				WC_EDIT,
				"",
				WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|WS_VSCROLL|ES_MULTILINE|ES_READONLY|ES_LEFT,
				MATRIX_X+COLUMN_SIZE*4+50,
				LINE_HEIGHT,
				1000,
				650,
				hWnd,
				(HMENU)EDIT_PROCESS_ID,
				hCurInstance,
				NULL
			);

			//fill show_thread_args
			show_thread_args.is_3x3 = TRUE;
			show_thread_args.matrix_elem = matrix_elem;
			show_thread_args.matrix_has_changed = false;
			show_thread_args.edit_equation_system = edit_equation_system;
			show_thread_args.combobox_method = combo_box_metodo;
			show_thread_args.edit_process = edit_process;
			pthread_mutex_init(&show_thread_args.matrix_has_changed_mutex,NULL);
			pthread_mutex_init(&show_thread_args.sleep_mutex,NULL);
			pthread_cond_init(&show_thread_args.sleep_condv,NULL);

			//start show_thread
			pthread_t show_thread;
			pthread_attr_t show_thread_attr;

			pthread_attr_init(&show_thread_attr);
			pthread_attr_setstacksize(&show_thread_attr,8000*1000);

			pthread_create(&show_thread,&show_thread_attr,show_thread_routine,NULL);

			pthread_attr_destroy(&show_thread_attr);

			send_signal_to_show_thread();

			break;
		case WM_CLOSE:

			pthread_mutex_destroy(&show_thread_args.matrix_has_changed_mutex);
			pthread_mutex_destroy(&show_thread_args.sleep_mutex);
			pthread_cond_destroy(&show_thread_args.sleep_condv);

			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	WNDCLASSEX WndClass =
	{
		.cbSize = sizeof(WndClass),
		.style = CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hCurInstance,
		.hIcon = LoadIcon(NULL,IDI_APPLICATION),
		.hCursor = LoadCursor(NULL,IDC_ARROW),
		.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH),
		.lpszMenuName = NULL,
		.lpszClassName = "MAINWND",
		.hIconSm = NULL
	};

	if(RegisterClassEx(&WndClass) == FALSE)
	{
		system("Error 1");
		return -1;
	}

	HWND hWnd = CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		"MAINWND",
		"Michaso",
		WS_BORDER|WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		HWND_DESKTOP,
		NULL,
		hCurInstance,
		NULL
	);

	ShowWindow(hWnd,SW_SHOWMAXIMIZED);

	MSG msg;
	while(GetMessage(&msg,hWnd,0,0) == TRUE)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
