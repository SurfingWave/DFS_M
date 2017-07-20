#include <stdio.h>
#include <stdlib.h>

using namespace std;
#define array_size 26014


int main(void)
{
	FILE *fp;	
	long int len_parallel;

	if((fp=fopen("Parallel_result.bin","rb"))==NULL)
	{
		printf("Can't open the file");
		exit(0);
	}

    long int Parallel_result[array_size];
	if(fread(Parallel_result,sizeof(long int),1,fp) != 1)
			printf("file read error\n");


    FILE *fp1;	
    long int len_serial;
	if((fp1=fopen("Serial_result.bin","rb"))==NULL)
	{
		printf("Can't open the file");
		exit(1);
	}
	

	long int Serial_result[array_size];
	if(fread(Serial_result,sizeof(long int),1,fp1) != 1)
		printf("file read error\n");
	fclose(fp);

	double correct = 0.0;
    for(long int i=0;i<array_size;i++)
    {
    	if(Serial_result[i] == Parallel_result[i])
    		correct++;
    }

    printf(" Correct: %f\n array_size: %d\n Correctness: %f\n", correct, array_size, correct/array_size);

	

	return 0;
}
 