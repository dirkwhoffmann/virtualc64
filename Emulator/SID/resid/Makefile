SRC=$(wildcard *.cc)
OBJ=$(SRC:.cc=.o)

.PHONY: all clean

all: $(OBJ)
	@echo > /dev/null

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f *.o

%.o: %.cc $(DEPS)
	@echo "Compiling $<"
	@$(MYCC) $(MYFLAGS) -c -o $@ $<
