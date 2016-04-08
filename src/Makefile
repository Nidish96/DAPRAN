CC=gcc
BIN_DIR=../bin
INC_DIR=../include
OBJ_DIR=../obj
LIB_DIR=../lib
CFLAGS=-L$(LIB_DIR) -lnid -lm -lfftw3 -I$(INC_DIR)
NIDFFT=nidfft
DEP=*.c $(INC_DIR)/*.h
.PHONY: clean all
all: $(DEP)
	cd $(OBJ_DIR) && make all && cd ../src && $(CC) -o $(BIN_DIR)/$(NIDFFT) $(NIDFFT).c $(CFLAGS) -g

debug: $(DEP)
	cd $(OBJ_DIR) && make debug && cd ../src && $(CC) -o $@ $(NIDFFT).c $(CFLAGS) -g

clean:
	rm -f debug $(BIN_DIR)/$(NIDFFT) && cd $(OBJ_DIR) && make $@
