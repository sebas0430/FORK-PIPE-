#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{

	//se abre el archivo00.txt en modo de lectura
	FILE *archivo00 = fopen(argv[2],"r");
	//se imprime error en caso de no poder abrir correctamente el archivo
	if(archivo00 == NULL)
	{
	perror("error al abrir el archivo");
	}
	//cantidad de elementos a leer dentro del archivo00
	int numero00 = atoi(argv[1]);
	//se crea el arreglo dinamico y se le ingresan los valores
	int *arreglo00 = malloc(numero00 * sizeof(int));
	for (int i = 0; i < numero00; i++)
	{
	fscanf(archivo00, "%d", &arreglo00[i]);
	}
//--------------------------------------------------------------------------------------
       //se abre el archivo01.txt en modo de lectura
        FILE *archivo01 = fopen(argv[4],"r");
        //se imprime error en caso de no poder abrir correctamente el archivo
        if(archivo01 == NULL)
        {
        perror("error al abrir el archivo");
        }
        //cantidad de elementos a leer dentro del archivo01
        int numero01 = atoi(argv[3]);
        //se crea el arreglo dinamico y se le ingresan los valores
        int *arreglo01 = malloc(numero01 * sizeof(int));
        for (int i = 0; i < numero01; i++)
        {
        fscanf(archivo01, "%d", &arreglo01[i]);
        }
//---------------------------------------------------------------------------------
//procesos
    int pipeNieto[2];     // nieto -> primer hijo
    int pipeSegundo[2];   // segundo hijo -> primer hijo
    int pipeTotal[2];     // primer hijo -> padre

    pipe(pipeNieto);
    pipe(pipeSegundo);
    pipe(pipeTotal);

int primerHijo = fork();
if(primerHijo == 0)
{

//cierro los canales de escritura del segundo hijo y del nieto
close(pipeSegundo[1]);
close(pipeNieto[1]);

int contador00;
int contador01;

read(pipeSegundo[0],&contador01,sizeof(int));
read(pipeNieto[0],&contador00,sizeof(int));

int total = contador01  + contador00;

close(pipeTotal[0]);
write(pipeTotal[1],&total,sizeof(int));
write(pipeTotal[1],&contador01,sizeof(int));
write(pipeTotal[1],&contador00,sizeof(int));


close(pipeTotal[1]);

exit(0);



}//aca termina el primer hijo



//segundo hijo
int segundoHijo = fork();
if(segundoHijo == 0)
{
   int pipNietoInterno[2];
   pipe(pipNietoInterno);

//nieto
int nieto = fork();
if(nieto == 0)
{
int contador00 = 0;
for(int i = 0; i<numero00;i++)
	{contador00+=arreglo00[i];}

	close(pipNietoInterno[0]);
	write(pipNietoInterno[1], &contador00, sizeof(int));
	close(pipNietoInterno[1]);
	exit(0);
}//aca termina el nieto

//leer lo del nieto y guardar su contador dentro del segundo hijo
int contador00;
close(pipNietoInterno[1]);
read(pipNietoInterno[0],&contador00,sizeof(int));
close(pipNietoInterno[0]);
//enviar al primer hijo el resultado del nieto
close(pipeNieto[0]);
write(pipeNieto[1],&contador00,sizeof(int));
close(pipeNieto[1]);

//suma de segundo hijo
int contador01 = 0;
for(int i = 0 ; i< numero01 ;i++)
{
	contador01+=arreglo01[i];
}
//se envia lo del segundo al primer hijo
	close(pipeSegundo[0]);
	write(pipeSegundo[1],&contador01,sizeof(int));
	close(pipeSegundo[1]);
	wait(NULL);
	exit(0);
}//aca termina el segundo hijo

    close(pipeNieto[1]);
    close(pipeNieto[0]);
    close(pipeSegundo[1]);
    close(pipeSegundo[0]);
    close(pipeTotal[1]);

    int resultadoFinal;
    int resultadoSegundoHijo;
    int resultadoNieto;

    read(pipeTotal[0], &resultadoFinal, sizeof(int));
    read(pipeTotal[0], &resultadoSegundoHijo, sizeof(int));
    read(pipeTotal[0], &resultadoNieto, sizeof(int));

    printf("\nResultado del nietoo: %d\n", resultadoNieto);
    printf("\nResultado del segundo hijo: %d\n", resultadoSegundoHijo);
    printf("\nResultado final desde primer hijo: %d\n", resultadoFinal);

    close(pipeTotal[0]);
    wait(NULL); // primer hijo
    wait(NULL); // segundo hijo

	fclose(archivo00);
	fclose(archivo01);
	free(arreglo01);
	free(arreglo00);

return 0;
}
