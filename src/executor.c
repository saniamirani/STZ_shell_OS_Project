#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

static void close_pipes(int fd[][2], int n)
{
    for (int i = 0; i < n; i++) { close(fd[i][0]); close(fd[i][1]); }
}

static void wait_child(pid_t pid, int number)
{
    int status;
    pid_t result;
    do { result = waitpid(pid, &status, 0); } while (result == -1 && errno == EINTR);
    if (result == -1) { perror("waitpid"); return; }
    if (debug_mode && WIFEXITED(status))
        fprintf(stderr, "[DEBUG] Command %d exit code: %d\n", number, WEXITSTATUS(status));
    else if (debug_mode && WIFSIGNALED(status))
        fprintf(stderr, "[DEBUG] Command %d signal: %d\n", number, WTERMSIG(status));
}

void execute_pipeline(Pipeline *p)
{
    int fd[MAX_COMMANDS-1][2], n = p->count - 1, made = 0, started = 0;
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < n; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe"); close_pipes(fd, made); return;
        }
        made++;
        debug_log("pipe() created");
    }

    for (int i = 0; i < p->count; i++) {
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); break; }
        if (pid == 0) {
            signal(SIGINT, SIG_DFL);
            if (i > 0 && dup2(fd[i-1][0], STDIN_FILENO) == -1) {
                perror("dup2 input"); _exit(1);
            }
            if (i < n && dup2(fd[i][1], STDOUT_FILENO) == -1) {
                perror("dup2 output"); _exit(1);
            }
            close_pipes(fd, made);
            if (debug_mode) fprintf(stderr, "[DEBUG] exec: %s\n", p->commands[i].argv[0]);
            execvp(p->commands[i].argv[0], p->commands[i].argv);
            perror("execvp"); _exit(127);
        }
        pids[started++] = pid;
        if (debug_mode) fprintf(stderr, "[DEBUG] forked PID %d\n", pid);
    }
    close_pipes(fd, made);
    for (int i = 0; i < started; i++) wait_child(pids[i], i+1);
}
