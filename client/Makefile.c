CC = gcc
CFLAGS = -Wall -Wextra -I../common
CLI_SRCS = agence.c interface.c
CLI_OBJS = $(CLI_SRCS:.c=.o)
CLI_EXEC = agence

all: $(CLI_EXEC)

$(CLI_EXEC): $(CLI_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLI_OBJS) $(CLI_EXEC)

.PHONY: all clean
