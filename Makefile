LIB_DIR=lib/
INC_DIR=include/
BIN_DIR=bin/
SRC_DIR=src/
OBJ_DIR=obj/

CC=gcc
CFLAGS=-L$(LIB_DIR) -lnid -lm -lfftw3 -I$(INC_DIR)

LIBNID=libnid.a
NIDFFT=nidfft

.PHONY: all clean debug
all: $(BIN_DIR)$(NIDFFT)
debug: $(BIN_DIR)debug

$(BIN_DIR)$(NIDFFT): $(LIB_DIR)$(LIBNID) $(SRC_DIR)$(NIDFFT).c
	$(CC) -o $@ $(SRC_DIR)$(NIDFFT).c $(CFLAGS)

$(BIN_DIR)debug: $(LIB_DIR)libdbgnid.a $(SRC_DIR)$(NIDFFT).c
	$(CC) -o $@ $(SRC_DIR)$(NIDFFT).c $(CFLAGS) -g

$(OBJ_DIR)ColumnRead.o: $(INC_DIR)ColumnRead.c
	$(CC) -c $^ -o $@

$(OBJ_DIR)ColumnRead_dbg.o: $(INC_DIR)ColumnRead.c
	$(CC) -c $^ -o $@ -g

$(LIB_DIR)$(LIBNID): $(OBJ_DIR)ColumnRead.o
	$(RM) -f $@
	$(AR) ru $@ $^
	ranlib $@

$(LIB_DIR)libdbgnid.a: $(OBJ_DIR)ColumnRead_dbg.o
	$(RM) -f $@
	$(AR) ru $@ $^
	ranlib $@
clean:
	rm -f $(BIN_DIR)$(NIDFFT) $(OBJ_DIR)* $(LIB_DIR)$(LIBNID) $(BIN_DIR)debug 
