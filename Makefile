CC = clang
CFLAGS += -Wall -Wextra -Werror --std=c17

pfusch: pfusch.c

clean:
	rm -f pfusch *.o

.PHONY: clean
