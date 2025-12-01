CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -I./src
SRCDIR := src

ifeq ($(OS),Windows_NT)
    TARGET := codex.exe
    RM := del /Q /F
else
    TARGET := codex
    RM := rm -f
endif

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))

$(TARGET): $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(OBJ)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) main.o $(TARGET)

.PHONY: clean