#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define TRUE 1
#define FALSE 0

pthread_t *tid;
int *threadFree;

int cpyFile(const char* src_path, const char* dst_path, int xwr)
{
	int src_fd, dst_fd, n, err;
	int countBite = 0;
   unsigned char buffer[4096];
	src_fd = open(src_path, O_RDONLY);
   dst_fd = open(dst_path, O_CREAT|O_WRONLY);
	if (src_fd == -1)
	{
			fprintf(stderr, "Error open input file.\n");
         exit(EXIT_FAILURE);
    }
	if (dst_fd == -1)
	{
			fprintf(stderr, "Error open output file.\n");
         exit(EXIT_FAILURE);
    }
	while (1) 
	{
		err = read(src_fd, buffer, 4096);
      if (err == -1) 
		{
			fprintf(stderr, "Error reading file.\n");
         exit(EXIT_FAILURE);
      }
		n = err;
		countBite +=err;
      if (err == 0) break;
		err = write(dst_fd, buffer, n);
      if (err == -1) 
		{
			fprintf(stderr, "Error writing to file.\n");
         exit(EXIT_FAILURE);
      }
	}
	close(src_fd);
   close(dst_fd);
	chmod(dst_path, xwr);
	return countBite;
}


typedef struct _infoForThreads{
	char path_file_1[PATH_MAX];
	char path_file_2[PATH_MAX];
	int mode;
	int numberThreads;
}infoForThreads;


void* threadsWorking(void *args) {
	infoForThreads *info = (infoForThreads*)args;
	int countByte = cpyFile(info->path_file_1, info->path_file_2, info->mode);
	fprintf (stderr, "tid = %d, pid=%d, path: %s, count copybyte: %d\n", gettid(), getpid(), info->path_file_2, countByte);
	threadFree[info->numberThreads] = TRUE;	
}

void createFullPath(char* fullPathDir, const char* nameDir, const char* nameFile)
{
	strcpy(fullPathDir, nameDir);	
	strcat(fullPathDir, nameFile);
}

int copyDir( char* path_Dir1, char* path_Dir2, int MAX_PROCESSES)
{
	infoForThreads *info;
	info = (infoForThreads*)calloc(MAX_PROCESSES, sizeof(infoForThreads));
	if (info == NULL)
	{
		fprintf(stderr, "Error create array info\n");
      exit(EXIT_FAILURE);
	} 

 
	DIR *ptrDir1 = opendir(path_Dir1); 
	DIR *ptrDir2 = opendir(path_Dir2);
	struct dirent *currFileDir1;
	struct stat st_direntDir1;
	struct dirent *currFileDir2;
	struct stat st_direntDir2;
	char  temp[PATH_MAX], path_file_1[PATH_MAX], path_file_2[PATH_MAX];

	strcat(path_Dir1, "/");
	strcat(path_Dir2, "/");

	if (ptrDir1 == NULL){
		fprintf(stderr,"Ошибка открытия каталога:%s", path_Dir1);
		return EXIT_FAILURE;
	} 
	if (ptrDir2 == NULL){
		fprintf(stderr,"Ошибка открытия каталога:%s", path_Dir2);
		return EXIT_FAILURE;
	}

	int i = 0;
	while((currFileDir1 = readdir(ptrDir1)) != NULL) 
	{
		
		createFullPath(path_file_1, path_Dir1, currFileDir1->d_name);
		if (!stat(path_file_1, &st_direntDir1)) 
		{
			if (S_ISREG(st_direntDir1.st_mode) )
			{
				rewinddir(ptrDir2);
				int isFind = FALSE; 
				while(currFileDir2 = readdir(ptrDir2))
				{
					createFullPath(path_file_2, path_Dir2, currFileDir2->d_name);
					if (!stat(path_file_2, &st_direntDir2))
					{
						if (S_ISREG(st_direntDir2.st_mode) && !strcmp(currFileDir2->d_name, currFileDir1->d_name))
						{
							isFind = TRUE;
							break;
						} 	
					}else 
					{
						fprintf(stderr, "Error statting %s: %s\n", path_file_2, strerror( errno ) );
					}
				}
				if (!isFind)
				{

					int i = 0;
					while (1){
						if (i == MAX_PROCESSES)
							i = 0;
						if ( threadFree[i] == TRUE )
							break;
						i++; 					
					}

					threadFree[i] = FALSE;

					createFullPath(temp, path_Dir2, currFileDir1->d_name);				
					
					info[i].numberThreads = i;
					strcpy(info[i].path_file_1, path_file_1);
					strcpy(info[i].path_file_2, temp);
					info[i].mode = st_direntDir1.st_mode;

 					int status = pthread_create(tid + i, NULL, &threadsWorking, info + i);
    				if (status != 0)
					{
       				printf("main error: can't create thread, status = %d\n", status);
        				exit(EXIT_FAILURE);
    				}
				}		
			}					
		}else
		{
			fprintf(stderr, "Error statting %s: %s\n", path_Dir1, strerror( errno ) ); 
			return EXIT_FAILURE;
		} 		
	}
	closedir(ptrDir1);
	closedir(ptrDir2);		
}

int main(int argc, char *argv[]){
	if (argc < 4)
	{
		puts("wrong number of parameters");
		return 0;
	}
	char *path_dir_1 = realpath(argv[1], NULL); 
	char *path_dir_2 = realpath(argv[2], NULL);

	if (path_dir_1 == NULL) {
		fprintf(stderr,"Ошибка открытия каталога:%s, %s", argv[1], strerror(errno));
    	return EXIT_FAILURE;
	} 
	if (path_dir_2 == NULL) {
		fprintf(stderr,"Ошибка открытия каталога:%s, %s", argv[2], strerror(errno));
		return EXIT_FAILURE;
	}
	int MAX_PROCESSES = atoi(argv[3]);  
	if ( MAX_PROCESSES == 0)
	{
		fprintf(stderr, "Неверное число аргументов");
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
	
	copyDir(path_dir_1, path_dir_2, MAX_PROCESSES);
	
	for (int i = 0; i < MAX_PROCESSES; i++)
       pthread_join(tid[i], NULL);

	free(tid);

	return EXIT_SUCCESS;
}
