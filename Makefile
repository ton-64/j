CC = cc
CFLAGS = -std=c99 -g

.PHONY: test
	
test: TEST.c
	$(CC) $(CFLAGS) $< -o TEST
	trap 'rm -f TEST' EXIT; ./TEST
