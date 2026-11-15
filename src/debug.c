#include <stdio.h>
#include <string.h>
#include "shell.h"

//track whether Debug Mode is enabled
int debug_mode =0;

int toggle_debug(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count){
		//toggle Debug Mode
		if (command_count == 1 && strcmp(commands[0][0], "debug") == 0){
			debug_mode = !debug_mode;
			if (debug_mode)
				printf("[DEBUG] Debug Mode enabled.\n");
			else
				printf("[DEBUG] Debug Mode disabled.\n");
				
			return 1;
		}
		return 0;
}

void log_debug_commands(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count){
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
}