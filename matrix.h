#pragma once

#include "num.h"

#define NXN_MAXN 3

#define MATRIX_MAXNROWS NXN_MAXN*2-1
#define MATRIX_MAXNCOLUMNS NXN_MAXN+1

typedef struct
{
        num_t matrix[MATRIX_MAXNROWS][MATRIX_MAXNCOLUMNS];
        size_t nrows;
        size_t ncolumns;
        bool is_3x3;
}matrix_t;