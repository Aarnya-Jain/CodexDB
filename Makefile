CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -I./src
SRCDIR := src

ifeq ($(OS),Windows_NT)
    TARGET := codex.exe
else
    TARGET := codex
endif

SRC := $(shell find $(SRCDIR) -name '*.cpp')
OBJ := $(SRC:.cpp=.o)

$(TARGET): $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(OBJ)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main.o $(TARGET)

.PHONY: clean
