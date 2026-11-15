#include <stdio.h>
#include <string.h>
#include "shell.h"

int parse_input(char *input, char *commands[MAX_COMMANDS][MAX_ARGS], int *command_count_out){
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
			return -1;
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
			return -1;
		}
			
		//validate pipeline
		if (command_count ==0 || command_count != pipe_count +1){
			printf("Invalid pipeline\n");
			return -1;
		} 

		*command_count_out = command_count;
		return 0;
}