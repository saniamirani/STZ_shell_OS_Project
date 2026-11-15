#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

//function to close all created pipes
void close_pipes(int fd[][2], int count){
	for(int i=0; i< count; i++){
		close(fd[i][0]);
		close(fd[i][1]);
	}
}

void execute_commands(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count){
		//normal command execution
		if (command_count == 1){
			pid_t pid = fork();
			
			if (pid<0){
				perror("fork failed");					
				if(debug_mode)
					fprintf(stderr, "[DEBUG] fork() failed\n");
			}
			else if (pid ==0){
				signal(SIGINT, SIG_DFL);
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
			 	return;
			 
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
					signal(SIGINT, SIG_DFL);
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
}