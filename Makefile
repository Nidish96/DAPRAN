LIB_DIR=lib/
INC_DIR=include/
BIN_DIR=bin/
SRC_DIR=src/
OBJ_DIR=obj/
EX_DIR=examples/

DBG=-O
CC=gcc $(DBG)
CFLAGS=-L$(LIB_DIR) -lnid -lm -lfftw3 -I$(INC_DIR)

LIBNID=libnid.a
NIDFFT=nidfft
NIDSTFT=nidstft

.PHONY: all clean debug example
all: $(BIN_DIR)$(NIDFFT) $(BIN_DIR)$(NIDSTFT)
example: $(BIN_DIR)$(NIDFFT) $(BIN_DIR)$(NIDSTFT)
	cp $(BIN_DIR)* $(EX_DIR)

$(BIN_DIR)$(NIDFFT): $(LIB_DIR)$(LIBNID) $(SRC_DIR)$(NIDFFT).c
	$(CC) -o $@ $(SRC_DIR)$(NIDFFT).c $(CFLAGS)

$(BIN_DIR)$(NIDSTFT): $(LIB_DIR)$(LIBNID) $(SRC_DIR)$(NIDSTFT).c
	$(CC) -o $@ $(SRC_DIR)$(NIDSTFT).c $(CFLAGS)

$(OBJ_DIR)ColumnRead.o: $(INC_DIR)ColumnRead.c
	$(CC) -c $^ -o $@

$(OBJ_DIR)Windows.o: $(INC_DIR)Windows.c
	$(CC) -c $^ -o $@

$(LIB_DIR)$(LIBNID): $(OBJ_DIR)ColumnRead.o $(INC_DIR)*.h $(OBJ_DIR)Windows.o
	$(RM) -f $@
	$(AR) ru $@ $(OBJ_DIR)ColumnRead.o $(OBJ_DIR)Windows.o
	ranlib $@

clean:
	rm -f $(BIN_DIR)$(NIDFFT) $(OBJ_DIR)* $(LIB_DIR)$(LIBNID) $(BIN_DIR)debug 
