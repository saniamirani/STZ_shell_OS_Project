#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

//built-in commands (cd, safe, procs, monitor)
static int extra_builtins(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count)
{
    if (command_count != 1)
        return 0;

    if (strcmp(commands[0][0], "cd") == 0) {
        const char *path = commands[0][1] ? commands[0][1] : getenv("HOME");
        if (!path || chdir(path) == -1)
            perror("cd");
        return 1;
    }

    if (strcmp(commands[0][0], "safe") == 0) {
        safe_mode = !safe_mode;
        printf("Safe Mode %s.\n", safe_mode ? "enabled" : "disabled");
        return 1;
    }

    if (strcmp(commands[0][0], "procs") == 0) {
        process_manager();
        return 1;
    }

    if (strcmp(commands[0][0], "monitor") == 0) {
        system_monitor();
        return 1;
    }

    return 0;
}

void shell_loop(void){
	char *username=getenv("USER");

	if (username == NULL) {
	username = "user";
	}

	char input[INPUT_SIZE];
	
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

		if (parse_input(input, commands, &command_count) == -1)
			continue;

		if (toggle_debug(commands, command_count))
			continue;

		if (extra_builtins(commands, command_count))
			continue;

		if (!is_command_allowed(commands, command_count))
			continue;

		log_debug_commands(commands, command_count);

		execute_commands(commands, command_count);
	}//outermost while end
}