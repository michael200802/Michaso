#ifndef SW_PROCESS_IN_EDIT
#define SW_PROCESS_IN_EDIT

#include <windows.h>
#include <windowsx.h>
#include <pthread.h>

#include "matrix.h"

#include "text.h"

typedef const HWND hedit_t;

//cramer
void show_process_cramer(hedit_t edit, matrix_t matrix);
//cramer

//gauss
matrix_t * show_process_gauss(hedit_t edit, matrix_t matrix);
//gauss

//gauss-jordan
void show_process_gauss_jordan(hedit_t edit, matrix_t matrix);
//gauss-jordan

#endif