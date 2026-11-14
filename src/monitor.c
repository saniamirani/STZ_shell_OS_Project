#include <stdio.h>
#include <string.h>
#include "shell.h"

int system_monitor(void)
{
    char line[256];
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) { perror("/proc/meminfo"); return 1; }
    printf("===== STZ System Monitor =====\n");
    while (fgets(line, sizeof(line), f))
        if (!strncmp(line, "MemTotal:", 9) || !strncmp(line, "MemAvailable:", 13))
            printf("%s", line);
    fclose(f);
    f = fopen("/proc/loadavg", "r");
    if (f) {
        if (fgets(line, sizeof(line), f)) printf("CPU Load Average: %s", line);
        fclose(f);
    }
    printf("==============================\n");
    return 0;
}
