SUBDIRS = \
Utilities Base Cartridges CIA CPU FileSystems LogicBoard Media Memory \
Peripherals RetroShell SID VICII

CXX        = g++
CC         = $(CXX)
CCFLAGS    = -std=c++17 -O3 -Wall -Wfatal-errors
CPPFLAGS   = -I $(CURDIR)/.. $(addprefix -I, $(shell find $(CURDIR) -type d))

export CC CCFLAGS CPPFLAGS

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

.PHONY: all prebuild subdirs clean bin

all: prebuild $(OBJ) subdirs
	@echo > /dev/null
	
prebuild:
	@echo "Entering ${CURDIR}"
		
subdirs:
	@for dir in $(SUBDIRS); do \
		echo "Entering ${CURDIR}/$$dir"; \
		$(MAKE) -C $$dir; \
	done

clean:
	@echo "Cleaning up $(CURDIR)"
	@rm -f virtualC64 *.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

bin:
	@echo "Linking"
	g++ -pthread -o virtualC64 ../Utilities/*.o *.o */*.o */*/*.o

%.o: %.cpp $(DEPS)
	@echo "Compiling $<"
	@$(CC) $(CCFLAGS) $(CPPFLAGS) -c -o $@ $<
