#include <stdio.h>
#include <stdlib.h>
#include "_num.h"

int main(int argc, char * argv[])
{
    argc--;
    argv++;
    if(argc < 2)
    {
        puts("Ingrese dos numeros o mas.");
        return -1;
    }

    size_t nnums = argc;
    num_t * arr_nums = (num_t*)malloc(nnums*sizeof(num_t));
    num_t product;
    char buffer[100];

    for(size_t i = 0; i < nnums; i++)
    {
        strtonum(argv[i],&arr_nums[i]);
    }

    for(size_t i = 0; i < nnums; i++)
    {
        printnum(buffer,&arr_nums[i]);
        printf("%zu. %s\n",i,buffer);

        for(size_t j = 0; j < nnums; j++)
        {
            if(i == j)
            {
                continue;
            }
            printnum(buffer,&arr_nums[j]);
            printf("\t%zu. %s\n",j,buffer);

            printf("\t\tCompare: ");
            switch(compare_num(&arr_nums[i],&arr_nums[j]))
            {
                case NUM_GREATER:
                    puts("Greater");
                    break;
                case NUM_LESSER:
                    puts("Lesser");
                    break;
                case NUM_EQUAL:
                    puts("Equal");
            }

            printf("\t\tSum: ");
            sum_num(&arr_nums[i],&arr_nums[j],&product);
            printnum(buffer,&product);
            puts(buffer);

            printf("\t\tRest: ");
            rest_num(&arr_nums[i],&arr_nums[j],&product);
            printnum(buffer,&product);
            puts(buffer);

            printf("\t\tMul: ");
            multiply_num(&arr_nums[i],&arr_nums[j],&product);
            printnum(buffer,&product);
            puts(buffer);

            printf("\t\tDiv: ");
            divide_num(&arr_nums[i],&arr_nums[j],&product);
            printnum(buffer,&product);
            puts(buffer);
        }

    }
    

    free(arr_nums);

    return 0;
}