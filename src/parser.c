
#include <stdio.h>
#include <string.h>
#include "shell.h"

/* Check for empty commands in a pipeline */
static int valid_pipeline(char *input)
{
    int has_command = 0;

    for (int i = 0; input[i] != '\0'; i++) {

        if (input[i] == '|') {

            if (!has_command) {
                fprintf(stderr,
                        "Error: invalid pipeline.\n");
                return 0;
            }

            has_command = 0;
        }
        else if (input[i] != ' ' && input[i] != '\t') {
            has_command = 1;
        }
    }

    if (!has_command) {
        fprintf(stderr, "Error: invalid pipeline.\n");
        return 0;
    }

    return 1;
}

/* Simple whitespace parser. Quoted arguments and redirection are future extensions. */
int parse_input(char *input, Pipeline *p)
{
    char *save1 = NULL, *part;

    p->count = 0;

    /* Validate before splitting */
    if (!valid_pipeline(input))
        return -1;

    part = strtok_r(input, "|", &save1);

    while (part) {
        char *save2 = NULL, *tok;
        int n = 0;

        if (p->count >= MAX_COMMANDS) {
            fprintf(stderr,
                    "Error: maximum %d commands allowed.\n",
                    MAX_COMMANDS);
            return -1;
        }

        tok = strtok_r(part, " \t", &save2);

        while (tok) {
            if (n >= MAX_ARGS - 1) {
                fprintf(stderr,
                        "Error: maximum %d arguments per command.\n",
                        MAX_ARGS - 1);
                return -1;
            }

            p->commands[p->count].argv[n++] = tok;
            tok = strtok_r(NULL, " \t", &save2);
        }

        if (n == 0) {
            fprintf(stderr,
                    "Error: empty command in pipeline.\n");
            return -1;
        }

        p->commands[p->count].argv[n] = NULL;
        p->count++;

        part = strtok_r(NULL, "|", &save1);
    }

    return p->count ? 0 : -1;
}
