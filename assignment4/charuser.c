#include <unistd.h>
#include <time.h>
#include <stdlib.h> 
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include <sys/time.h>

#define N 100
#define MAX_LEN 256

#define EXIT_FAILURE 1
void usleep(unsigned long usec);

int main(int argc, char* argv[])
{

	time_t t;
	srand((unsigned) time(&t));
	int fd, n, i;

	//Producer: 
	if(strcmp(argv[1], "p")== 0){
		fd = open("/dev/mypipe", O_WRONLY);
		if(fd < 0)
		{
			perror("**ERROR** Open");
			return -1;
		}	

		for (i = 0;i < N;i++) { //How many times we produce a string			
			usleep(1000 * (rand() % 1000)); 	
	
			char string[256];
			sprintf(string, "012356789abcdefgh %d", i); //Sends our string to string var 
			printf("Producer generated string %d\n", i);

			int size_of_string = strlen(string)+1;

			n = write( fd, string, size_of_string);
			if(n < 0)
			{
				perror("**ERROR** Write");
				return -1;
			}
		}
		if(close(fd)<0)
		{
			perror("**ERROR** Close");
			return -1;
		}
	}

	//Consumer:
	if(strcmp(argv[1], "c")== 0){
		fd = open("/dev/mypipe", O_RDONLY);
		if(fd < 0)
		{
			perror("**ERROR** Open");
			return -1;
		}

		for (i = 0;i < N;i++) {
			usleep(1000 * (rand() % 1000));

			char* string = (char *)calloc(MAX_LEN, sizeof(char));
				
			n = read(fd, string, MAX_LEN);
			if(n < 0)
			{
				perror("**ERROR** Read");
				return -1;
			}

			printf("Consumer iteration %d: %s\n", i, string);
	
		}

		if(close(fd)<0)
		{
			perror("**ERROR** Close");
			return -1;
		}
	}


	return 0;

}
