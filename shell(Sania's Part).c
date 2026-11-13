#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 10
#define INPUT_SIZE 200

//function to close all created pipes
void close_pipes(int fd[][2], int count){
	for(int i=0; i< count; i++){
		close(fd[i][0]);
		close(fd[i][1]);
	}
}

int main(void){
	char *username=getenv("USER");

	if (username == NULL) {
	username = "user";
	}

	char input[INPUT_SIZE];
	
	//track whether Debug Mode is enabled
	int debug_mode =0;
	
	while(1){
		printf("%s@STZ-Shell:~$ ", username);
		fflush(stdout);

		if(fgets(input, sizeof(input), stdin)== NULL){
			break;
		}
		
		//detect input longer than the buffer
		if (strchr(input, '\n')== NULL){
			int ch=getchar();
			
			if(ch != '\n' && ch !=EOF){
				while (ch != '\n' && ch != EOF){
					ch = getchar();
				}
				
				printf("Error: Input is too long!\n");
				continue;
			}
		}
				
		
		//remove newline character from input
		input[strcspn(input, "\n")] = '\0';
		
		//ignore empty or whitespace-only input
		if (strspn(input, " \t") == strlen(input)){
			continue;
		}
		
		if (strcmp(input,"exit")==0)
			break;
		    
		//Array to store arguments of multiple commands
		char *commands[MAX_COMMANDS][MAX_ARGS];
		
		int command_count =0;
		int pipe_count =0;
		int too_many_args=0;
		
		//count pipe symbols
		for( int k=0; input[k] != '\0'; k++) {
			if(input[k]== '|'){
			pipe_count++;
			}
		}
		
		//reject more than 10 commands
		if (pipe_count >= MAX_COMMANDS) {
			printf("Error: Maximum 10 commands allowed!!\n");
			continue;
		}
		
		//split input into individual commands
		char *saveptr1;
		char *command= strtok_r(input, "|", &saveptr1);
		
		while(command !=NULL && command_count < MAX_COMMANDS){
			int j=0;
			//tokenize each individual command
			char *saveptr2;
			char *token= strtok_r(command, " \t", &saveptr2);
			
			while(token!=NULL){
			
				//detect arguments exceeding the limit
				if (j>=MAX_ARGS - 1){
					too_many_args = 1;
					break;
				}
				
				commands[command_count][j]=token;
				j++;
				token=strtok_r(NULL, " \t", &saveptr2);
			}
			
			 if (too_many_args){
			 	break;
			 }
			 
			commands[command_count][j] = NULL; 
			if (j==0)
				break;
			command_count++;
			
			command = strtok_r(NULL, "|", &saveptr1);
		}//tokenization end
		
		//reject commands with too many arguments
		if (too_many_args){
			printf("Error: Too many arguments! Maximum is %d per command, including the command name.\n", 
				MAX_ARGS - 1);
			continue;
		}
			
		//validate pipeline
		if (command_count ==0 || command_count != pipe_count +1){
			printf("Invalid pipeline\n");
			continue;
		} 
		
		//toggle Debug Mode
		if (command_count == 1 && strcmp(commands[0][0], "debug") == 0){
			debug_mode = !debug_mode;
			if (debug_mode)
				printf("[DEBUG] Debug Mode enabled.\n");
			else
				printf("[DEBUG] Debug Mode disabled.\n");
				
			continue;
		}
		
		//log commands and arguments when Debug Mode is enabled
		if(debug_mode){
			printf("\n[DEBUG] Commands received:\n");
			
			for(int i=0; i < command_count; i++){
				printf("[DEBUG] Command %d: ", i+1);
				
				for(int j=0; commands[i][j] !=NULL; j++){
					printf("%s ", commands[i][j]);
				}
				
				printf("\n");
			}
			printf("\n");
		}
			
		//normal command execution
		if (command_count == 1){
			pid_t pid = fork();
			
			if (pid<0){
				perror("fork failed");					
				if(debug_mode)
					fprintf(stderr, "[DEBUG] fork() failed\n");
			}
			else if (pid ==0){
				//log execution attempt
				if(debug_mode)
					fprintf(stderr, "[DEBUG] execvp() executing: %s\n", commands[0][0]);
					
				execvp(commands[0][0], commands[0]);
				
				if(debug_mode)
					fprintf(stderr, "[DEBUG] execvp() failed: %s\n", commands[0][0]);
				
				perror("execvp failed");
				_exit(127);
			}
			else{
				//log successful fork in parent
					if (debug_mode){
						fprintf(stderr, "[DEBUG] fork() succeeded: PID %d\n", pid);
					}
				//capture and display command return code
				int status;
				if(waitpid(pid, &status, 0) == -1){
					perror("waitpid failed");
					if(debug_mode)
						fprintf(stderr, "[DEBUG] waitpid() failed: PID %d\n", pid);				
				}
				else if(debug_mode){
					if (WIFEXITED(status))
						printf("[DEBUG] Exit code: %d\n", WEXITSTATUS(status));
					else if (WIFSIGNALED(status))
						printf("[DEBUG] Terminated by signal: %d\n", WTERMSIG(status));
				}
			}
		}
		//multiple-command pipeline execution
		else{
			//create required pipes
			int fd[MAX_COMMANDS - 1][2];
			
			int pipes_created=0;
			 
			 for(int i=0; i<command_count - 1 ; i++){
			 	if(pipe(fd[i]) == -1){
			 		perror("Pipe failed");
			 		if (debug_mode)
			 			fprintf(stderr, "[DEBUG] pipe() failed: pipe %d\n", i+1);
			 					 			
			 		//close previously created pipes
			 		close_pipes(fd, pipes_created);
			 		
			 		pipes_created=0;
			 		break;
			 	}
			 	
			 	//log successful pipe creation
			 	if(debug_mode)
			 		fprintf(stderr, "[DEBUG] pipe() succeeded: pipe %d\n", i+1);
			 	
			 	pipes_created++;
			 }
			 // stop if pipe creation failed
			 if (pipes_created != command_count -1)
			 	continue;
			 
			 //store child process IDs
			 pid_t pids[MAX_COMMANDS];
			 
			 int created=0;
			 
			 //create a child for every commands
			 for( int i=0; i < command_count; i++){
			 
				 pid_t pid =fork();
				 
				 if (pid< 0){
				 	perror("fork failed");
				 	if(debug_mode)
				 		fprintf(stderr, "[DEBUG] fork() failed for command %d\n", i+1);
				 		
				 	break;
				}				
				if (pid==0){
					//connect input from previous pipe
					if(i>0){
						if(dup2(fd[i-1][0], STDIN_FILENO) == -1) {
							perror("dup2 input failed");
							if(debug_mode)
								fprintf(stderr, "[DEBUG] dup2() input redirection failed\n");
															
							_exit(1);
						}
						//log successful input redirection
						if(debug_mode)
							fprintf(stderr, "[DEBUG] dup2() input redirection succeeded\n");
					}
					//connect output to next pipe
					if(i< command_count -1){
						if(dup2(fd[i][1], STDOUT_FILENO) == -1) {
							perror("dup2 output failed");
							if(debug_mode)
								fprintf(stderr, "[DEBUG] dup2() output redirection failed\n");
														
							_exit(1);
						}
						//log successful output redirection
						if(debug_mode)
							fprintf(stderr, "[DEBUG] dup2() output redirection succeeded\n");
					}
					//close all original file descriptors
					close_pipes(fd, pipes_created);
					
					//log execution attempt
					if(debug_mode)
						fprintf(stderr, "[DEBUG] execvp() executing: %s\n", commands[i][0]);
						
					//execute current command
					execvp(commands[i][0], commands[i]);
					
					if(debug_mode)
						fprintf(stderr, "[DEBUG] execvp() failed: %s\n", commands[i][0]);
					
					perror("execvp failed");
					_exit(127);
				}
				else{
					pids[created] = pid;
					created++;
					
					//log successful fork in parent
					if (debug_mode){
						fprintf(stderr, "[DEBUG] fork() succeeded: PID %d\n", pid);
					}
				}
			}
			//parent closes all pipe descriptors
			close_pipes(fd, pipes_created);
			
			//capture return code of every pipeline command
			for(int i=0; i<created; i++){
				int status;
				
				if(waitpid(pids[i], &status, 0) == -1){
					perror("waitpid failed");
					if(debug_mode)
						fprintf(stderr, "[DEBUG] waitpid() failed: PID %d\n", pids[i]);				
				}
				else if (debug_mode){
					if(WIFEXITED(status))
						printf("[DEBUG] Command %d exit code: %d\n",i + 1, WEXITSTATUS(status));
					else if (WIFSIGNALED(status))
						printf("[DEBUG] Command %d terminated by signal: %d\n", i+1, WTERMSIG(status));
				}
			}								
		}// multiple commands else end            
	}//outermost while end
	return 0;
}



















	
