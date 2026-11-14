#include <stdio.h>
#include "shell.h"
int debug_mode = 0;
void debug_log(const char *s) { if (debug_mode) fprintf(stderr, "[DEBUG] %s\n", s); }
