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

check_filesystem:
 @echo "#include <filesystem>" > fs_test.cpp
 @$(CXX) -std=c++17 -c fs_test.cpp -o fs_test.o 2>nul || \
 ( echo "ERROR: Your compiler does NOT support <filesystem>." && \
   echo "Install MinGW-w64 (WinLibs GCC 12+)." && \
   $(RM) fs_test.cpp 2>nul && false )
 @$(RM) fs_test.cpp 2>nul
 @$(RM) fs_test.o 2>nul

$(TARGET): check_filesystem $(OBJ) main.o
 $(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(OBJ)

main.o: main.cpp
 $(CXX) $(CXXFLAGS) -c main.cpp -o main.o

%.o: %.cpp
 $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
 $(RM) $(OBJ) main.o $(TARGET)

.PHONY: clean check_filesystem
