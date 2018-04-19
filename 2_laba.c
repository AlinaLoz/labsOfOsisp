#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <linux/limits.h>


int search(const char *name_dir, const char *name_search_file, int *countWatchingElement)
{
	DIR *ptrDir=opendir(name_dir); //директория
	struct dirent *currFile;   //просто по айлам и название
	struct stat st_dirent;   //конрке инфа о файлн
	char fullPath[PATH_MAX] = "";	
	if (ptrDir != NULL){
		while((currFile=readdir(ptrDir)) != NULL){
			strcpy(fullPath, name_dir);
			strcat(fullPath, "/");		
			strcat(fullPath, currFile->d_name);
			if (!stat(fullPath, &st_dirent)) {
				if (  strcmp( currFile->d_name, ".")  &&  strcmp( currFile->d_name, "..") ) {
					(*countWatchingElement)++;
					if (S_ISDIR(st_dirent.st_mode)){
						search(fullPath, name_search_file, countWatchingElement);						
					}else if (S_ISREG(st_dirent.st_mode) ){
						if (!strcmp(name_search_file, currFile->d_name) ){
							printf("path: %s; size: %ld; time create: %s, I-node number: %ld, mode: %lo (octal)\n", fullPath, (long)st_dirent.st_size, ctime(&st_dirent.st_mtime), (long)st_dirent.st_ino, (unsigned long)st_dirent.st_mode);
							printf("count watching element: %d\n", *countWatchingElement);
							return 1;
						}		
					}
				}
			}else{
			 	printf( "Error statting %s: %s\n", fullPath, strerror( errno ) );
				return 0;
			}	
		}
	}		
	else{
		printf( "Error opening %s: %s", name_dir, strerror( errno ) );
	}	
	closedir(ptrDir);	
	return 0;
}

int main(int argc, char *argv[]){
	if (argc < 3)
	{
		puts("wrong number of parameters");
		return 0;
	}
	int countWatchingElement = 0;
	if (!search(argv[1], argv[2],  &countWatchingElement)) 
		printf( "file not found\n");
	return 0;
}
