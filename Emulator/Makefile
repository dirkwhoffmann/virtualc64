SUBDIRS = \
Utilities Base Cartridges CIA CPU Datasette Drive FileSystems Files LogicBoard Memory  \
Peripherals RetroShell SID VICII

MYCC = g++ -O2 -std=c++17 -Wfatal-errors -Wall
MYFLAGS = \
-I $(CURDIR)/.. \
-I $(CURDIR) \
-I $(CURDIR)/Utilities \
-I $(CURDIR)/Base \
-I $(CURDIR)/Cartridges \
-I $(CURDIR)/Cartridges/CustomCartridges \
-I $(CURDIR)/CIA \
-I $(CURDIR)/CPU \
-I $(CURDIR)/Datasette \
-I $(CURDIR)/Drive \
-I $(CURDIR)/FileSystems \
-I $(CURDIR)/Files \
-I $(CURDIR)/LogicBoard \
-I $(CURDIR)/Memory \
-I $(CURDIR)/Peripherals \
-I $(CURDIR)/RetroShell \
-I $(CURDIR)/SID \
-I $(CURDIR)/SID/resid \
-I $(CURDIR)/SID/fastsid \
-I $(CURDIR)/VICII

export MYCC MYFLAGS

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
	@$(MYCC) $(MYFLAGS) -c -o $@ $<
