CC = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L -g -O0
LDLIBS = -lcurl -lcjson
TARGET = speedtest
DEPS = download.h location.h options.h serverlist.h upload.h utils.h
PREFIX = /usr/local

SRC = main.c download.c upload.c location.c options.c serverlist.c utils.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin

clean: 
	rm -f $(TARGET) $(OBJ)

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
