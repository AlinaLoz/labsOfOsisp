//http://www.csc.villanova.edu/~mdamian/threads/posixsem.html
#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h> 
#include <pthread.h>

#define SIZE_BUFFER 20
#define TRUE 1
#define FALSE 0
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

sem_t sem_name;

pthread_t *tid;
int *threadFree;

typedef struct _infoForThreads{
	char buffer[SIZE_BUFFER];
	char path_file_2[PATH_MAX];
	int numberThreads;
	int pos_from_1_file;
}infoForThreads;

int add_new_string(int pos_2, int pos_1, int count_byte)
{
	FILE *f_out;
	if ( (f_out = fopen("output.txt", "ab")) == NULL ){
		fprintf(stderr,"temp file, %s\n", strerror(errno));
    	return EXIT_FAILURE;	
	}
	char output_string[30] = {'\0'};
	char temp[10] = {'\0'};
	sprintf(temp, "%d", pos_2);
	strcpy(output_string, temp);
	strcat(output_string, ";");
	sprintf(temp, "%d", pos_1);
	strcat(output_string, temp);
 	strcat(output_string, ";");
	sprintf(temp, "%d", count_byte);
	strcat(output_string, temp);
	fprintf(f_out, "%s\n", output_string);
	fclose(f_out);
	return EXIT_SUCCESS;
}

void* threadsWorking(void *args) {
	infoForThreads *info = (infoForThreads*)args;
	
	FILE *f, *f_out;
	int curr_pos = 0;
	if ( (f = fopen(info->path_file_2, "r")) == NULL )
		fprintf(stderr,"File 2 error open:, %s\n", strerror(errno));

	char *buffer = (char*)calloc(SIZE_BUFFER, sizeof(char));
	if (buffer == NULL)
		fprintf(stderr,"Error buffer:, %s\n", strerror(errno));

	int err;
	while (1){	
		err = fread(buffer, sizeof(char), SIZE_BUFFER, f);
		
		if (err == 0) break;
		if (memcmp(buffer, info->buffer, err) == 0)
		{
			sem_wait(&sem_name);
			int count_string = 0;
			add_new_string(curr_pos, info->pos_from_1_file, err);
			sem_post(&sem_name); 
		}  
		curr_pos += err; 
	}
	threadFree[info->numberThreads] = TRUE;	
	fclose(f);
}

int check_on_plagiat(char *name_f_1, char *name_f_2, int MAX_PROCESSES)
{
	infoForThreads *info;
	info = (infoForThreads*)calloc(MAX_PROCESSES, sizeof(infoForThreads));
	if (info == NULL)
	{
		fprintf(stderr, "Error create array info\n");
      exit(EXIT_FAILURE);
	} 
	
	FILE *f_1;

	if ( (f_1 = fopen("output.txt", "w")) == NULL ){
		fprintf(stderr,"Error rewrite file, %s\n", strerror(errno));
    	return EXIT_FAILURE;		
	}

	if ( (f_1 = fopen(name_f_2, "r")) == NULL ){
		fprintf(stderr,"File 2 error open:, %s\n", strerror(errno));
    	return EXIT_FAILURE;	
	}else fclose(f_1);

	if ( (f_1 = fopen(name_f_1, "r")) == NULL ){
		fprintf(stderr,"File 1 error open:, %s\n", strerror(errno));
    	return EXIT_FAILURE;	
	}

	char *buffer = (char*)calloc(SIZE_BUFFER, sizeof(char));
	if (buffer == NULL){
		fprintf(stderr,"Error buffer:, %s\n", strerror(errno));
    	return EXIT_FAILURE;	
	}
	int err = 0;
	int curr_pos = 0;

	while (1)
	{
		err = fread(buffer, sizeof(char), SIZE_BUFFER, f_1); 
		 
		if (err == 0) break;
		
		int i = 0;
		while (1){
			if (i == MAX_PROCESSES)
			i = 0;
			if ( threadFree[i] == TRUE )
				break;
				i++; 					
		}
		threadFree[i] = FALSE;

		info[i].numberThreads = i;
		strcpy(info[i].path_file_2, name_f_2);	
		info[i].pos_from_1_file = curr_pos;
		memcpy(info[i].buffer, buffer, SIZE_BUFFER);

		int status = pthread_create(tid + i, NULL, &threadsWorking, info + i);
    	if (status != 0)
		{
       	printf("main error: can't create thread, status = %d\n", status);
        	exit(EXIT_FAILURE);
    	}	
		curr_pos += err; 
	}	
	fclose(f_1);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	if (argc < 4)
	{
		fprintf(stderr,"count of parametrs is wrong.\n");
		return EXIT_FAILURE;
	}
	FILE *f_1;
	if ( (f_1 = fopen(argv[1], "r")) == NULL ){
		fprintf(stderr,"File not found in this directory:%s, %s\n", argv[1], strerror(errno));
    	return EXIT_FAILURE;	
	}

	if ( (f_1 = fopen(argv[2], "r")) == NULL ){
		fprintf(stderr,"File not found in this directory:%s, %s\n", argv[2], strerror(errno));
    	return EXIT_FAILURE;	
	}

	int MAX_PROCESSES = atoi(argv[3]);  
	if ( MAX_PROCESSES == 0 )
	{
		fprintf(stderr, "counts of threads is wrong");
    	return EXIT_FAILURE;
	}

	tid = (pthread_t*)calloc(MAX_PROCESSES, sizeof(pthread_t));
	threadFree = (int*)calloc(MAX_PROCESSES, sizeof(int));
	if (tid == NULL || threadFree == NULL)
	{
		fprintf(stderr, "Error create array threads\n");
      exit(EXIT_FAILURE);
	} 

	for (int i = 0; i < MAX_PROCESSES; i++){
		tid[i] = 0;
		threadFree[i] = TRUE;
	}

 	sem_init(&sem_name, 0, 1);

	check_on_plagiat(argv[1], argv[2], MAX_PROCESSES);

	for (int i = 0; i < MAX_PROCESSES; i++)
       pthread_join(tid[i], NULL);

	sem_destroy(&sem_name);
	free(tid);
	return EXIT_SUCCESS;
}
