// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
	int fd1[2]; // Used to store two ends of first pipe 
	int fd2[2]; // Used to store two ends of second pipe 
    char *argv1[] = {"ps", "aux", NULL};
    char *argv2[] = {"sort", "-nrk", "3,3", NULL};
    char *argv3[] = {"head", "-5", NULL};

	pid_t p; 

	if (pipe(fd1)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 
	if (pipe(fd2)==-1) 
	{ 
		fprintf(stderr, "Pipe Failed" ); 
		return 1; 
	} 

	p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 

	// Parent process 
	else if (p == 0) 
	{ 
          
        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        execv("/bin/ps", argv1); 
	} 

	// child process 
	else
	{ 
        wait(NULL);
        pid_t p2;
        p2 = fork();

        if(p2 < 0){
            fprintf(stderr, "fork Failed" ); 
		    return 1; 
        }

        else if(p2==0){
            close(fd1[1]);
            dup2(fd1[0], STDIN_FILENO);

            close(fd2[0]);
            dup2(fd2[1], STDOUT_FILENO);

            execv("/bin/sort", argv2);
        }

        else{
            close(fd1[0]);
            close(fd1[1]);

            close(fd2[1]);
            dup2(fd2[0], STDIN_FILENO);
            close(fd2[0]);

            execv("/bin/head", argv3);
        }

		exit(0); 
	} 
} 