#include <signal.h>
#include "shell.h"
/* The shell stays open on Ctrl+C. Child commands restore default SIGINT behavior. */
void setup_shell_signals(void) { signal(SIGINT, SIG_IGN); }
