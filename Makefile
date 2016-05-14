LIB_DIR=lib/
INC_DIR=include/
BIN_DIR=bin/
SRC_DIR=src/
OBJ_DIR=obj/

CC=gcc
CFLAGS=-L$(LIB_DIR) -lnid -lm -lfftw3 -I$(INC_DIR)

LIBNID=libnid.a
NIDFFT=nidfft

.PHONY: all clean
all: $(BIN_DIR)$(NIDFFT)

$(BIN_DIR)$(NIDFFT): $(LIB_DIR)$(LIBNID) $(SRC_DIR)$(NIDFFT).c
	$(CC) -o $@ $(SRC_DIR)$(NIDFFT).c $(CFLAGS)

$(OBJ_DIR)ColumnRead.o: $(INC_DIR)ColumnRead.c
	$(CC) -c $^ -o $@

$(LIB_DIR)$(LIBNID): $(OBJ_DIR)ColumnRead.o
	$(RM) -f $@
	$(AR) ru $@ $^
	ranlib $@

clean:
	rm -f $(BIN_DIR)$(NIDFFT) $(OBJ_DIR)* $(LIB_DIR)$(LIBNID) 
