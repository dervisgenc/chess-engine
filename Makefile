CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Iinclude

SRC = src/main.cpp src/board.cpp src/attacks.cpp src/movegen.cpp
TARGET = chess-engine

all: $(TARGET)

$(TARGET):$(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean: 
	rm -f $(TARGET)

.PHONY: all run clean