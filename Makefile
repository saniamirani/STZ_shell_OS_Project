CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude -g
TARGET = stz-shell
SOURCES = src/main.c src/shell.c src/parser.c src/executor.c src/process_manager.c src/monitor.c src/signals.c src/safe_mode.c src/debug.c

all: $(TARGET)

$(TARGET): $(SOURCES) include/shell.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
