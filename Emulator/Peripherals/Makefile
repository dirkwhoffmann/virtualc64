SUBDIRS = Drive Datasette Joystick Mouse Keyboard

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

.PHONY: all clean subdirs

#all: $(OBJ) subdirs
all: subdirs
	@echo > /dev/null

subdirs:
	@for dir in $(SUBDIRS); do \
		echo "Entering ${CURDIR}/$$dir"; \
		$(MAKE) -C $$dir; \
	done

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f *.o
		@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	@$(MYCC) $(MYFLAGS) -c -o $@ $< $(CFLAGS)
