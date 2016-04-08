OBJ_DIR=../obj
.PHONY: clean
libnid.a: $(OBJ_DIR)/*.o
	$(RM) -f $@
	$(AR) ru $@ $^
	ranlib $@

clean:
	rm -f libnid.a
