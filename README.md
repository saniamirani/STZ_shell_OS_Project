# STZ Shell

A beginner-friendly custom Linux shell written in C for Ubuntu.

## Requirements
- Ubuntu/Linux
- GCC and Make

Install build tools:
```bash
sudo apt update
sudo apt install build-essential
```

## Build and run
```bash
make
./stz-shell
```
Or use `make run`.

## Commands
| Command | Purpose |
|---|---|
| `ls`, `pwd`, `date` | Normal Linux commands |
| `cd path` | Change directory |
| `exit` | Exit the shell |
| `debug` | Toggle Debug Mode |
| `safe` | Toggle Safe Mode |
| `procs` | List processes from `/proc` |
| `monitor` | Show memory information and CPU load average |
| `ls \| wc -l` | Example pipeline |

## Team responsibilities
- **Sania:** command parsing, command execution, pipelines, Debug Mode.
- **Taraz:** process information, system monitoring, signal handling, Safe Mode.
- **Shahzaib:** README, setup instructions, and documentation.

## Current limitations
This first version intentionally keeps parsing simple. Quoted strings, redirection (`>`, `<`, `>>`), command history, background jobs, and continuously refreshing monitoring are not implemented. Safe Mode blocks a small list of command names and is a demonstration feature, not a security boundary.
