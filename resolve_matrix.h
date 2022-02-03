#include "num.h"
#include "text.h"
#include <stdbool.h>
#include <stdlib.h>

#ifndef RESOLVE_MATRIX_H
#define RESOLVE_MATRIX_H

#define NXN_MAXN 3

typedef struct
{
        num_t matrix[NXN_MAXN*2-1][NXN_MAXN+1];
        size_t nrows;
        size_t ncolumns;
        bool is_3x3;
}matrix_t;

typedef enum method {CRAMER = 1,GAUSS = 2, JORDAN = 4} method_t;

text_t matrix_to_system(matrix_t matrix);

text_t cramer(matrix_t matrix);

text_t gauss(matrix_t matrix);

text_t jordan(matrix_t matrix);

#endif
