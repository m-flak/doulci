CXXFLAGS=-std=c++17 -Wall -Wextra -g3
INCLUDES=-I./include/
LIBS=-luWS

SRC_FILES = main.cpp \
	    doulci.cpp
BIN_FILE=doulci

DOX_DIR=docs

.PHONY: all clean docs clean-docs

all: $(BIN_FILE)

$(BIN_FILE):
	g++ $(CXXFLAGS) $(INCLUDES) -o $(BIN_FILE) $(SRC_FILES) $(LIBs);
	echo -e " Run \'make docs\' to make source tree documentations... \n";

clean: clean-docs
	rm -f *.o;
	rm -f $(BIN_FILE)

clean-docs: 
	make -C docs clean

docs:
	make -C docs all

