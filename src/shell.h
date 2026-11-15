#ifndef SHELL_H
#define SHELL_H

#define MAX_COMMANDS 10
#define MAX_ARGS 10
#define INPUT_SIZE 200

extern int debug_mode;
extern int safe_mode;

// shell_loop.c
void shell_loop(void);

// parser.c 
int parse_input(char *input, char *commands[MAX_COMMANDS][MAX_ARGS], int *command_count_out);

// executor.c 
void close_pipes(int fd[][2], int count);
void execute_commands(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count);

// debug.c 
int toggle_debug(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count);
void log_debug_commands(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count);

// signals.c 
void setup_shell_signals(void);

// safe.c 
int is_command_allowed(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count);

// procs.c 
int process_manager(void);

// monitor.c 
int system_monitor(void);

#endif