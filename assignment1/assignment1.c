#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "vector.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define debug 0

#define true 1
#define false 0

int argsCount = 0;
int bmode = false;
char* inFile =  NULL; 
char* outFile = NULL;

void doPipeCommands(Vector cmds[], int pipes){ 
	const int commands = pipes + 1;   //number of commands to run
	int i = 0;

	int pipefds[2 * pipes];

	for(i = 0; i < pipes; i++){
		if(pipe(pipefds + i * 2) < 0) {
			perror("**ERROR** Failed to pipe");
			exit(EXIT_FAILURE);
		}
	}
	
	int pid;
	int status;

	int j = 0;

	for (i = 0; i < commands; ++i) {
		if((pid = fork()) == -1){
			perror("**ERROR** Fork failed"); 
			exit(EXIT_FAILURE); 
		}else if(pid == 0) {
			/**FILE REDIRECTION CHECK**/
			if(outFile != NULL) {
				int output = open(outFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
				if(output == -1) { 
					perror("**ERROR** File output failed"); 
					exit(EXIT_FAILURE); 
				}
				if(dup2(output, 1) == -1) { 
					perror("**ERROR** Output dup2 failed");  
					exit(EXIT_FAILURE); 
				}
			}
			
			if(inFile != NULL) {
				int input = open(inFile, O_RDONLY, 0644);
				if(input == -1) { 
					perror("**ERROR** Failed to open input file");
					exit(EXIT_FAILURE); 
				}
				if(dup2(input, 0) == -1) { 
					perror("**ERROR** Input dup2 failed"); 
					exit(EXIT_FAILURE); 
				}
			}
			
			/**EXECUTE PIPELINE COMMANDS**/
			if(i < pipes){  //if not last command
				if(dup2(pipefds[j + 1], 1) < 0){
					perror("**ERROR** Dup2 failed");
					exit(EXIT_FAILURE);
				}
			}

			if(j != 0 ){  //if not first command && j != 2 * pipes
				if(dup2(pipefds[j - 2], 0) < 0){
					perror("**ERROR** Dup2 failed");
					exit(EXIT_FAILURE);
				}
			}

			int k;
			for(k = 0; k < 2 * pipes; k++){
				close(pipefds[k]);
			}
        
			if(execvp(vec_get_str(&cmds[i],0), get_arr(&cmds[i])) < 0){
				perror("**ERROR** Failed to execute pipeline command");
				exit(EXIT_FAILURE);
			}
		}
		j += 2;
	}
	
	for(i = 0; i < 2 * pipes; i++){
		close(pipefds[i]);
	}
	
	for(i = 0; i < pipes + 1; i++){
		if(!bmode){ /**BACKGROUND MODE CHECK**/
			wait(&status); 
		}
	}
}


void pipeline(Vector* args){
	/**PARSING**/
	if(debug) printf("argsCount: %d\n", argsCount);
	if(debug) print_vec(args,"args");
	
	int count = 1; //number of string vectors needed in array
	if(debug) printf("\n");
	
	int i;
	for(i = 0; i < argsCount; i++){
		if(strcmp(vec_get_str(args,i),"|") == 0){
			count++;
		}
	}
	
	if(debug) printf("We need %d string vectors\n",count);
	Vector cmds[count]; // array of string vectors
	
	for(i  = 0; i < count; i++){
			vec_init(&cmds[i],0); //initialize string vectors
	}
	if(debug) printf("\n");
	
	int cmdCount= 0;
	
	for(i = 0; i < argsCount; i++){
		if(strcmp(vec_get_str(args,i),"|") == 0){
			vec_append_str(&cmds[cmdCount],NULL);
			cmdCount++;
		}else{
			vec_append_str(&cmds[cmdCount],vec_get_str(args,i));
		}
	}
	
	vec_append_str(&cmds[cmdCount],NULL);
	
	for(i  = 0; i < count; i++){
		if(debug) print_vec(&cmds[i],"cmd");
		if(debug) printf("\n");
	}
	
	/**PIPELINE IMPLEMENTATION**/
	doPipeCommands(cmds,count - 1);
	
	for(i  = 0; i < count; i++){
		free_vec(&cmds[i], 1); 
	}
	
}

char* findFile(char* string, Vector* args){
	char* fileName = NULL;
	fileName = vec_file(args, string);
	if(fileName != NULL){
		argsCount -=2;
	}
	return fileName;
}

int execute(Vector args, int pline)
{
	if(isEmpty(&args)){
		return false;
	}
	
	if(strcmp(vec_get_str(&args,0),"e") == 0 && argsCount == 1){
		return false;
	}
		
	if(pline){ /**PIPELINE CHECK**/
		pipeline(&args);		
		return true;	
	}else{		
			pid_t  pid;
			int    status;
		  
			if((pid = fork()) == -1) { 
				perror("**ERROR** Fork failed"); 
				return false; 
			}else if(pid == 0) {
				/**FILE REDIRECTION CHECK**/
				if(outFile != NULL) {
					int output = open(outFile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
					if(output == -1) { 
					  perror("**ERROR** File output failed"); 
					  return false; 
					}
					if(dup2(output, 1) == -1) { 
					  perror("**ERROR** Output dup2 failed"); 
					  return false; 
					}
			    }
			
			    if(inFile != NULL) {
			      int input = open(inFile, O_RDONLY, 0644);
			      if(input == -1) { 
					  perror("**ERROR** Failed to open input file"); 
					  return false; 
					}  
			      if(dup2(input, 0) == -1) { 
					  perror("**ERROR** Input dup2 failed"); 
					  return false; 
					}
			    }
			    
				if(execvp(vec_get_str(&args,0), get_arr(&args)) == -1) { /**STANDARD EXECUTION**/
					perror("**ERROR** Command execute failed"); 
					return false; 
				}
				
			}else if(!bmode){ /**BACKGROUND MODE CHECK**/
				while (wait(&status) != pid); //wait for completion
			}
		
		  return true;
		  
	}
}

void shell(Vector args)
{
    printf("-> ");    
     
	int buf = 100;
	   char str[buf];
	   char* firstInput;
      
	   firstInput = fgets(str,buf,stdin);    
	   firstInput[strlen(firstInput) - 1] = '\0';
                 
      if(strcmp(firstInput,"cs350sh") == 0){
				
				int pline = false;
				char c;
				int valid = true;
				
				do{
				   printf("cs350sh> ");
					
					pline = false;
					
					if(!isEmpty(&args)){
						argsCount = 0;
						free_vec(&args, 0); // clear args
						vec_init(&args, 0); //re-init
					}
					
					/**PARSING**/
					c = ' ';
					
					while(c != '\n') {
					    int argCount = 0;
					    Vector arg;
						if(valid) vec_init(&arg, 1); //vector of chars
						   
						   for(c = getchar(); c != ' ' && c != '\n' && c != '\t'; c = getchar()) { //loops through each character
									vec_append_c(&arg, c);
									argCount++;
						    }
					    
					    vec_set_c(&arg, argCount, '\0');
					    
					    if(strcmp(get_string(&arg), "") != 0){
							valid = true;
							vec_append_str(&args, get_string(&arg));
							argsCount++; 
						}else{
							valid = false;
						}
						
					  } 
					  
					vec_set_str(&args,argsCount,NULL);
					  
					/**BACKGROUND MODE CHECK**/ 
					if((strcmp(vec_get_str(&args,argsCount - 1), "&")) == 0) {
						trim_vec(&args); //trim the "&" so arguments can be executed
						argsCount--;
						bmode = true;
					} else {
						bmode = false;
					}
					
					/**FILE REDIRECTION CHECK**/
					inFile = findFile("<", &args);
					outFile = findFile(">", &args);
					
					/**PIPELINE CHECK**/
					pline = has_string(&args,"|");
					
				}while(execute(args, pline));
					
				free_vec(&args,0);
					  
	  }else{
		  printf("Shell must be named \"cs350sh\"\n");
	  }         
}

int  main(void)
{
	Vector args;
	vec_init(&args, 0); //vector of strings
	shell(args);
	return 0;
}
