CXX := g++
INCLUDE_DIR := include
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

FUN_TARGET := $(BIN_DIR)/fun
FUN_OBJS := $(OBJ_DIR)/fun.o $(OBJ_DIR)/fun_lexer.o $(OBJ_DIR)/fun_parser.o $(OBJ_DIR)/fun_semantic.o $(OBJ_DIR)/fun_codegen.o

.PHONY: all clean

all: $(FUN_TARGET)

$(FUN_TARGET): $(FUN_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(FUN_OBJS) -o $(FUN_TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
