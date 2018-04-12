#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#define COUNT_SIGNAL 101
int children[9] = {0};
int count = 1;
int count_3 = 0;

void handler1(int sig)
{
	printf("%d : Process 1 / %d get signal SIGUSR2\n", count, getpid());
	if (count < COUNT_SIGNAL)
	{
		count++;
		FILE *f = fopen("./infa", "w");
		if (f != NULL)
			fclose(f);
		printf("Process 1 -> 8,7,6,5/ %d put signal SIGUSR1\n", getpid());	
		killpg( getpgid(children[8]), SIGUSR1);
	}
	else
	{
		for (int i=2; i<9;i++)
			kill(children[i], SIGKILL);
		exit(0);
	}
}

void handler2(int sig)
{
	printf("Process 2 / %d get signal SIGUSR2\n", getpid());	
	printf("Process 2 -> 1 / %d put signal SIGUSR2\n", getpid());
	kill(children[1], SIGUSR2);
}

void handler31(int sig)
{
	count_3++;	
	printf("Process 3 / %d get signal %d SIGUSR1\n", getpid(), count_3);
	
	if (count_3 == 4)
	{
		printf("Process 3 -> 2 / %d put signal SIGUSR2\n", getpid());
		kill(children[2], SIGUSR2);
		count_3 = 0;	
	}else
	{
		FILE *f;
		f = fopen("./infa", "w");
		if (f != NULL)
		{
			printf("Process 3  confirn send group\n");	
			fprintf(f, "%d\n", count_3);
			fclose(f);
		}
	}
}

void handler32(int sig)
{
	count_3++;	
	printf("Process 3 / %d get signal %d SIGUSR2\n", getpid(), count_3);
	if (count_3 == 4)
	{
		printf("Process 3 -> 2 / %d put signal SIGUSR2\n", getpid());
		kill(children[2], SIGUSR2);
		count_3 = 0;	
	}else
	{
		FILE *f;
		f = fopen("./infa", "w");
		if (f != NULL)
		{
			printf("Process 3  confirn send group\n");	
			fprintf(f, "%d\n", count_3);
			fclose(f);
		}
	}
}


void handler5(int sig)
{
	FILE *f;
	printf("Process 5 / %d get signal SIGUSR1\n", getpid());		
	do{	
		f = fopen("./infa", "r");
		if (f != NULL)
		{
			fscanf(f, "%d\n", &count_3);
			fclose(f);
		}
	}while(count_3 != 3);
	count_3 = 0;
	printf("Process 5 -> 3 / %d put signal SIGUSR2\n", getpid());
	kill(children[3], SIGUSR2);
}

void handler6(int sig)
{
	FILE *f;
	printf("Process 6 / %d get signal SIGUSR1\n", getpid());
	do{	
		f = fopen("./infa", "r");
		if (f != NULL)
		{
			fscanf(f, "%d\n", &count_3);
			fclose(f);
		}
	}while(count_3 != 2);
	count_3 = 0;
	printf("Process 6 -> 3 / %d put signal SIGUSR2\n", getpid());
	kill(children[3], SIGUSR2);
}


void handler7(int sig)
{
FILE *f;
	printf("Process 7 / %d get signal SIGUSR1\n", getpid());	
	do{	
		f = fopen("./infa", "r");
		if (f != NULL)
		{
			fscanf(f, "%d\n", &count_3);
			fclose(f);
		}
	}while(count_3 != 1);
	count_3 = 0;
	printf("Process 7 -> 3 / %d put signal SIGUSR2\n", getpid());
	kill(children[3], SIGUSR2);
}


void handler8(int sig)
{
	printf("Process 8 / %d get signal SIGUSR1\n", getpid());	
	printf("Process 8 -> 3 / %d put signal SIGUSR1\n", getpid());
	kill(children[3], SIGUSR1);
}



int main(int argc, char* argv[])
{	
	FILE *f = fopen("./infa", "w");
	if (f != NULL)
		fclose(f);
	int temp = 0;
	children[0] = getpid();
	int num = 0;
	for (int i = 1; i < 9; i++)
	{
		switch (i)
		{
			case 1:
				if (children[0] == getpid())
				{
					printf("Process 0 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{ 
							children[i] = getpid();
							num = 1;
					}
				}
				break;
			case 2:
				if (children[1] == getpid())
				{
					printf("Process 1 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							children[i] = getpid();
							num = 2;
					}
				}
			break;

			case 3:
				if (children[2] == getpid())
				{
					printf("Process 2 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							children[i] = getpid();
							num = 3;
					}
				}
			break;

			case 4:
				if (children[2] == getpid())
				{
					printf("Process 2 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							children[i] = getpid();
							num = 4;
					}
				}
			break;

			case 5:
				if (children[4] == getpid())
				{
					printf("Process 4 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							children[i] = getpid();
							num = 5;
							FILE *f;
							f = fopen("./infa_3", "w");
							if (f != NULL){
								for (int i=0;i<9;i++)
									fprintf(f, "%d\n", children[i]);
								fclose(f);
							}
					}
				}
			break;

			case 6:
				if (children[3] == getpid())
				{
					printf("Process 3 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							setpgrp(); 
							
							children[i] = getpid();
							num = 6;
					}else
					{
						setpgid(children[i], children[i]);
					}					
				}
			break;

			case 7:
				if (children[6] == getpid())
				{
					printf("Process 6 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{
							children[i] = getpid();
							setpgid(children[i], getpgid(getppid()));
							num = 7;
					}
				}
			break;

			case 8:
				if (children[7] == getpid())
				{
					printf("Process 7 = %d create %d\n", getpid(), i);
					if (!(children[i] = fork()))
					{ 
							children[i] = getpid();
							setpgid(children[i], getpgid(getppid()));
							num = 8;
							FILE *f, *f2;
							f = fopen("./infa_2", "w");
							f2 = fopen("./infa_3", "r");
							if (f != NULL && f2 != NULL)
							{
								for (int i=0;i<9;i++)
								{

									int temp;
									fscanf(f2,"%d\n", &temp);
									if (temp)
										fprintf(f, "%d\n", temp);
									else
										fprintf(f, "%d\n", children[i]);
								}	
								fclose(f);
								fclose(f2);
							}
					}
				}
			break;

		}
	}

	sleep(3);

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	switch (num)
	{
		case 1:
			act.sa_handler = handler1;
			sigaction(SIGUSR2, &act, 0);
			break;
		case 2:
			act.sa_handler = handler2;
			sigaction(SIGUSR2, &act, 0);
			break;
		case 3:
			act.sa_handler = handler31;
			sigaction(SIGUSR1, &act, 0);
			act.sa_handler = handler32;
			sigaction(SIGUSR2, &act, 0);			
			break;
		case 5:
			act.sa_handler = handler5;
			sigaction(SIGUSR1, &act, 0);
			break;
		case 6:
			act.sa_handler = handler6;
			sigaction(SIGUSR1, &act, 0);
			break;

		case 7:
			act.sa_handler = handler7;
			sigaction(SIGUSR1, &act, 0);
			break;

		case 8:	
			act.sa_handler = handler8;
			sigaction(SIGUSR1, &act, 0);				
			break;
	}

	sleep(3);

	if (num == 1)
	{ 
			int isNotFull = 1;
			FILE *f;
			while((f=fopen("./infa_2", "r")) == NULL) 
			{
				sleep(0.1);
			}
			do{
					int i = 0;
					fseek(f, 0, SEEK_SET); 
					isNotFull = 1;
					while (!feof(f))
					{
						fscanf(f, "%d\n", children + i);
						if (!children[i++]) 
						{
							isNotFull = 0;
							break;
						}		
					}
			}while(!isNotFull);
			fclose(f);
		while ( getpgid(children[6]) != getpgid(children[5]) ){};
		printf("Process 1 -> 8,7,6,5/ %d put signal SIGUSR1\n", getpid());
		killpg( getpgid(children[6]), SIGUSR1);
	}else if (num == 5)
	{
		FILE *f=fopen("./infa_2", "r");
		if (f != NULL)
		{
			for (int i = 0; i < 9; i++)
				fscanf(f, "%d\n", children + i);
			fclose(f);
			setpgid(children[5], getpgid(children[6]));
		}			
	} 
	if (num == 0) waitpid(children[1], NULL, 0);
	else	while(1){};
	return 0;
}
