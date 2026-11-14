#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "shell.h"

int process_manager(void)
{
    DIR *d = opendir("/proc");
    struct dirent *e;
    if (!d) { perror("/proc"); return 1; }
    printf("%-10s %-8s %s\n", "PID", "STATE", "PROCESS");
    while ((e = readdir(d))) {
        int numeric = e->d_name[0] != '\0';
        for (int i = 0; e->d_name[i]; i++)
            if (!isdigit((unsigned char)e->d_name[i])) numeric = 0;
        if (numeric) {
            char path[512], line[512], name[256] = "unknown", state[32] = "?";
            snprintf(path, sizeof(path), "/proc/%s/status", e->d_name);
            FILE *f = fopen(path, "r");
            if (!f) continue;
            while (fgets(line, sizeof(line), f)) {
                if (!strncmp(line, "Name:", 5)) sscanf(line+5, "%255s", name);
                if (!strncmp(line, "State:", 6)) sscanf(line+6, "%31s", state);
            }
            printf("%-10s %-8s %s\n", e->d_name, state, name);
            fclose(f);
        }
    }
    closedir(d);
    return 0;
}
