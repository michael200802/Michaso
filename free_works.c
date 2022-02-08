#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
	if(argc == 1)
	{
		system("echo \"No se ha indicado el tamaño del bloque de memoria.\"");
		exit(EXIT_FAILURE);
	}
	size_t size = strtol(*argv,NULL,10);
	system("echo \"No se ha reservado nada.\" && pause>0");
	void * mem[100];
	for(size_t i = 0; i < 100; i++)
	{
		 mem[i] = malloc(size);
	}
	for(size_t i = 0; i < 100; i++)
	{
		printf("%zu. Ptr:\n\t%p\n",i,mem[i]);
	}
	if(mem == NULL)
	{
		system("echo \"No se ha podido reservar memoria.\"");
		exit(EXIT_FAILURE);
	}
	system("echo \"Se ha reservado memoria.\" && pause>0");
	for(size_t i = 0; i < 100; i++)
	{
		free(mem[i]);
	}
	system("echo \"Se ha liberado la memoria\" && pause>0");
	return 0;
}
