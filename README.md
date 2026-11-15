# STZ Shell

STZ Shell is a small custom command-line shell written in C for Linux. It was developed as an Operating Systems Lab project to practise process creation, command execution, pipes, signals, and Linux process information.

## Requirements

- Ubuntu or another Linux distribution
- GCC compiler
- GNU Make
- Linux `/proc` filesystem (normally available on Ubuntu)

On Ubuntu, install the required build tools:

```bash
sudo apt update
sudo apt install build-essential
```

## Build and run

1. Open a terminal in the extracted `STZ_Shell_Submission` folder.
2. Compile the project:

```bash
make clean
make
```

3. Start STZ Shell:

```bash
./stz-shell
```

You can also build and start it with:

```bash
make run
```

To remove the compiled executable:

```bash
make clean
```

## Commands

| Command | Description |
|---|---|
| `ls`, `pwd`, `date`, `echo Hello` | Runs normal Linux commands |
| `cd path` | Changes the shell's current directory |
| `cd` | Changes to the user's home directory |
| `exit` | Closes STZ Shell |
| `debug` | Turns Debug Mode on or off |
| `safe` | Turns Safe Mode on or off |
| `procs` | Lists process IDs, states, and names using `/proc` |
| `monitor` | Displays memory information and system load average |
| `ls \| wc -l` | Example of a pipeline |

The prompt displays the current working directory. Paths inside the user's home directory are shown relative to `~`.

## Modes and tools

### Normal Mode
Normal Mode is the default. It accepts commands and runs external Linux programs.

### Debug Mode
Enter `debug` to enable or disable debug messages. When enabled, STZ Shell displays parsed commands and execution details such as process creation, pipe setup, command execution, and exit status.

### Safe Mode
Enter `safe` to enable or disable the basic command blocklist. While enabled, the shell blocks these command names:

`rm`, `rmdir`, `shutdown`, `reboot`, `mkfs`, `fdisk`, `wipefs`, `dd`, and `poweroff`.

Safe Mode is included for demonstration and learning. It is not a security boundary and should not be relied on to protect a real system.

### Monitor
Run `monitor` to display `MemTotal` and `MemAvailable` from `/proc/meminfo`, along with the load average from `/proc/loadavg`. This is a one-time snapshot, not a continuously refreshing monitor. Load average is not the same as CPU usage percentage.

### Process Manager
Run `procs` to list numeric process directories found under `/proc`, and display each process's PID, state, and name from `/proc/PID/status`.

## Keyboard interrupt

When a foreground command is running, press `Ctrl+C` to send an interrupt to that command. STZ Shell ignores `SIGINT` at its prompt, while child commands restore the default `SIGINT` behavior.

## Project structure

```text
STZ_Shell_Submission/
├── src/
│   ├── main.c
│   ├── shell_loop.c
│   ├── parser.c
│   ├── executor.c
│   ├── process_manager.c
│   ├── monitor.c
│   ├── signals.c
│   ├── safe.c
│   ├── debug.c
│   └── shell.h
├── tests/
│   └── README.md
├── Makefile
├── README.md
└── .gitignore
```

## Testing checklist

Run these commands inside STZ Shell:

```bash
pwd
ls -l
echo Hello World
ls | wc -l
cd ..
pwd
debug
date
debug
safe
rm sample.txt
safe
procs
monitor
exit
```

The following malformed pipelines should be rejected:

```bash
ls |
| ls
ls || wc
```

Enter the pipe character directly. Do not type a backslash before it.

## Current limitations

- The parser separates commands and arguments using spaces and tabs. Quoted arguments are not supported.
- Input/output redirection such as `>`, `<`, and `>>` is not implemented.
- Command history and background jobs are not implemented.
- Monitor displays a snapshot and does not continuously refresh.
- Process Manager lists process information but does not terminate or control processes.
- Safe Mode blocks a limited set of command names. It can be bypassed in other ways and is not a security mechanism.
- This is an educational shell and is not intended to replace a full-featured shell such as Bash.

## Team responsibilities

- **Sania Mirani:** command parsing, command execution, pipelines, and Debug Mode.
- **Muhammad Taraz:** process information, system monitoring, signal handling, and Safe Mode.
- **Shahzaib Mahar:** README, setup instructions, and documentation.

## Institution

Sukkur IBA University  
Operating Systems Lab, Section H

## Repository

https://github.com/saniamirani/STZ_shell_OS_Project/
