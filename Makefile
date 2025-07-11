# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

# Directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include

# Files
TARGET := $(BIN_DIR)/kvdb
SRCS := $(SRC_DIR)/main.cpp $(SRC_DIR)/kv_store.cpp $(SRC_DIR)/storage_engine.cpp

# Default rule
all: $(TARGET)

# Link and build executable
$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean run
