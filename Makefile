CXXFLAGS=-std=c++17 -Wall -Wextra -g3
INCLUDES=-I./include/

SRC_FILES = main.cpp \
	    doulci.cpp
BIN_FILE=doulci

.PHONY: all clean

all: $(BIN_FILE)

$(BIN_FILE):
	g++ $(CXXFLAGS) $(INCLUDES) -o $(BIN_FILE) $(SRC_FILES)

clean:
	rm -f *.o;
	rm -f $(BIN_FILE)

