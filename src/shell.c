
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

/* Display the actual current directory in the prompt */
static void show_prompt(const char *user)
{
    char cwd[4096];
    const char *home = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("%s@STZ-Shell:~$ ", user);
        return;
    }

    /* Replace HOME path with ~ */
    if (home && strncmp(cwd, home, strlen(home)) == 0 &&
        (cwd[strlen(home)] == '/' || cwd[strlen(home)] == '\0')) {

        printf("%s@STZ-Shell:~%s$ ",
               user, cwd + strlen(home));
    } else {
        printf("%s@STZ-Shell:%s$ ", user, cwd);
    }
}

static int builtin(Pipeline *p)
{
    char **a = p->commands[0].argv;

    if (p->count != 1) return 0;

    if (!strcmp(a[0], "exit")) exit(0);

    if (!strcmp(a[0], "cd")) {
        const char *path = a[1] ? a[1] : getenv("HOME");

        if (!path || chdir(path) == -1)
            perror("cd");

        return 1;
    }

    if (!strcmp(a[0], "debug")) {
        debug_mode = !debug_mode;
        printf("Debug Mode %s.\n",
               debug_mode ? "enabled" : "disabled");
        return 1;
    }

    if (!strcmp(a[0], "safe")) {
        safe_mode = !safe_mode;
        printf("Safe Mode %s.\n",
               safe_mode ? "enabled" : "disabled");
        return 1;
    }

    if (!strcmp(a[0], "procs")) {
        process_manager();
        return 1;
    }

    if (!strcmp(a[0], "monitor")) {
        system_monitor();
        return 1;
    }

    return 0;
}

void shell_loop(void)
{
    char input[INPUT_SIZE];
    const char *user = getenv("USER");

    if (!user) user = "user";

    while (1) {
        Pipeline p;

        show_prompt(user);
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            putchar('\n');
            break;
        }

        if (!strchr(input, '\n') && !feof(stdin)) {
            int c;

            while ((c = getchar()) != '\n' && c != EOF) {}

            puts("Error: input is too long.");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strspn(input, " \t") == strlen(input))
            continue;

        if (parse_input(input, &p) == -1)
            continue;

        if (builtin(&p))
            continue;

        if (!is_command_allowed(&p))
            continue;

        if (debug_mode) {
            for (int i = 0; i < p.count; i++) {
                fprintf(stderr, "[DEBUG] Command %d:", i + 1);

                for (int j = 0; p.commands[i].argv[j]; j++)
                    fprintf(stderr, " %s",
                            p.commands[i].argv[j]);

                fputc('\n', stderr);
            }
        }

        execute_pipeline(&p);
    }
}
