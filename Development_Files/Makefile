CC=gcc
CFLAGS=-L../lib -lnid -lm -lfftw3 -I../include
EXEC=nidfft
.PHONY: clean
$(EXEC): Main.c
	$(CC) -o $@ $^ $(CFLAGS)

debug: Main.c
	$(CC) -o $@ $^ $(CFLAGS) -g

clean:
	rm -f $(EXEC) debug
