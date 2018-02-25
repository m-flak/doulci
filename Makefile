CXXFLAGS=-std=c++17 -Wall -Wextra -g3
INCLUDES=-I./include/

BIN_FILE=doulci

.PHONY: all clean

all: $(BIN_FILE)

$(BIN_FILE):
	g++ $(CXXFLAGS) $(INCLUDES) -o $(BIN_FILE) main.cpp

clean:
	rm -f *.o;
	rm -f $(BIN_FILE)

