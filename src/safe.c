#include <stdio.h>
#include <string.h>
#include "shell.h"

int safe_mode = 0;

int is_command_allowed(char *commands[MAX_COMMANDS][MAX_ARGS], int command_count)
{
    const char *blocked[] = {"rm","rmdir","shutdown","reboot","mkfs","fdisk",
                             "wipefs","dd","poweroff",NULL};
    if (!safe_mode) return 1;
    for (int i = 0; i < command_count; i++)
        for (int j = 0; blocked[j]; j++)
            if (!strcmp(commands[i][0], blocked[j])) {
                printf("Safe Mode: '%s' is blocked.\n", blocked[j]);
                return 0;
            }
    return 1;
}