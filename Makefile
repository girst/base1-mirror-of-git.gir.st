# base1
# (C) 2016 Tobias Girstmair, http://isticktoit.net/
# released under the GNU GPL v3. See LICENSE for details.

.PHONY: all clean test
CC=gcc
CFLAGS=-std=c99 -Werror -Wall -Wextra

all: base1

base1:
	$(CC) $(CFLAGS) main.c -o base1

clean:
	rm -f base1
