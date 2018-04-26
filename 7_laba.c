#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h> 
#include <math.h> 
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define COUNT_CIRCLE_MESSGE  1
#define SIZE_BLOCK_SEND_MESSAGE  3


char * decToBin(int x)
{
	char *t = (char *)malloc(1 * SIZE_BLOCK_SEND_MESSAGE + 1);
	int i = 0;
	do
	{
		t[i++] = x % 2 + '0';  
		x = x / 2;
	} while (x != 0);
	t[i] = '\0';

	int k = strlen(t) - 2;
	int m = k / 2;
	for (int i = 0; i <= m; i++)
	{
		char tmp = t[i];
		t[i] = t[k - i + 1];
		t[k - i + 1] = tmp;
	}
	while(strlen(t) < 3)
	{
		char tmp[4] = "0";
		strcat(tmp, t);
		strcpy(t, tmp);
	}
	return t;
}

int producer(int num_producer, int from[], char* str){
	int val_sem;

	FILE *f;
	do{	
		f = fopen("./infa", "r");
		if (f != NULL)
		{
			fscanf(f, "%d\n", &val_sem);
			fclose(f);
		}
		if (val_sem == 0) 
			return EXIT_SUCCESS;
		sleep(0.1);
	}while(val_sem != num_producer);

	int buffer[SIZE_BLOCK_SEND_MESSAGE];
	
	close(from[0]);
	if ( strlen(str) > pow(2, SIZE_BLOCK_SEND_MESSAGE) - 1 )
	{	
		memset(buffer, '1', SIZE_BLOCK_SEND_MESSAGE); 
		write(from[1], buffer, SIZE_BLOCK_SEND_MESSAGE);
		write(from[1], str, pow(2, SIZE_BLOCK_SEND_MESSAGE));
		return EXIT_SUCCESS;
	}
	char *t = decToBin(strlen(str));
	
	write(from[1], t, SIZE_BLOCK_SEND_MESSAGE);
	write(from[1], str, strlen(str));
	f = fopen("./infa", "w");
	if (f != NULL)
	{
		fprintf(f, "%d\n", val_sem  + 1);
		fclose(f);
	}
	return EXIT_SUCCESS;
}

int binToDec(char ch[]){
	int n, sum = 0;
 	for (int i = SIZE_BLOCK_SEND_MESSAGE - 1; i > -1; i--)
   if ( ch[i] == '1' ) 
   	sum += pow( 2, i ) ;
	return sum;
}

int customer(int number_customer, int from[]){
	int val_sem;

	FILE *f;
	do{	
		f = fopen("./infa", "r");
		if (f != NULL)
		{
			fscanf(f, "%d\n", &val_sem);
			fclose(f);
		}
		if (val_sem == 0) 
			return EXIT_SUCCESS;
		sleep(0.1);
	}while(val_sem != number_customer);	
	
	if ( val_sem == 5 ) {
		f = fopen("./infa", "w");
		fprintf(f, "0\n");	
		fclose(f);
		return EXIT_SUCCESS;
	}
	char reading_string[1 * SIZE_BLOCK_SEND_MESSAGE + 1];
	char buffer[SIZE_BLOCK_SEND_MESSAGE + 1];
	fprintf(stderr,"customer %d read next string: ", number_customer);
	close(from[1]);
	read(from[0], buffer, SIZE_BLOCK_SEND_MESSAGE);
	buffer[SIZE_BLOCK_SEND_MESSAGE] = '\0';
	sleep(0.1);
	int size_message = binToDec(buffer);
	read(from[0], reading_string, size_message);	
	reading_string[size_message] = '\0';
	printf("%s\n", reading_string);

	f = fopen("./infa", "w");
	if (f != NULL)
	{
		fprintf(f, "%d\n", val_sem  + 1);
		fclose(f);
	}
	return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
	if (argc < 3)
	{
		puts("wrong number of parameters");
		return 0;
	}

	FILE *f = fopen("./infa", "w");
	if (f != NULL){
		fprintf(f, "1\n");
		fclose(f);
	}

	int arr_pid[6] = {0};
	int num = 0;
	int from[2];
	pid_t pid; 

	arr_pid[0] = getpid();
	

	if ( pipe(from) == -1 )
	{	
		fprintf(stderr,"Error create channel: %s", strerror(errno));
    	return EXIT_FAILURE;
	}

	for (int i = 1; i < 6; i++)  
	{
		switch (i){
			case 1:
				if (getpid() == arr_pid[0])
				{
					printf("Process 0 = %d create %d\n", getpid(), i);
					if (!(pid = fork ()))
					{				
						arr_pid[i] = getpid();
						num = 1; 
					}
				}
				break;
			case 2:
				if (getpid() == arr_pid[1])
				{
					printf("Process 1 = %d create %d\n", getpid(), i);
					if (!(pid = fork ()))
					{				
						arr_pid[i] = getpid();
						num = 2; 
					} 
				}
				break;
			case 3:	
				if (getpid() == arr_pid[0])
				{
					printf("Process 0 = %d create %d\n", getpid(), i);
					if (!(pid = fork ()))
					{				
						arr_pid[i] = getpid();
						num = 3; 
					} 
				}
				break;	
			case 4:	
				if (getpid() == arr_pid[3])
				{
					printf("Process 3 = %d create %d\n", getpid(), i);
					if (!(pid = fork ()))
					{				
						arr_pid[i] = getpid();
						num = 4; 
					}
				}
				break;
			case 5:	
				if (getpid() == arr_pid[0])
				{
					printf("Process 0 = %d create %d\n", getpid(), i);
					if (!(pid = fork ()))
					{				
						arr_pid[i] = getpid();
						num = 5; 
					}
				}
				break;
		}
	}
	sleep(2);

	for (int i = 0; i < COUNT_CIRCLE_MESSGE; i++)
		if ( num == 1 || num == 3 )
			producer(num, from, (num == 1) ? argv[1] : argv[2]);
		else
			customer(num, from);

	if ( num == 0 || num == 1 || num == 3 )
		while (wait(NULL) > 0){};

	sleep(0.2);
	return EXIT_SUCCESS;
} 
