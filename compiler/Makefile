CXX := g++
INCLUDE_DIR := include
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

CMD_TARGET := $(BIN_DIR)/cmd
CMD_OBJS := $(OBJ_DIR)/cmd.o $(OBJ_DIR)/cmd_lexer.o $(OBJ_DIR)/cmd_parser.o $(OBJ_DIR)/cmd_semantic.o $(OBJ_DIR)/cmd_codegen.o

.PHONY: all clean

all: $(CMD_TARGET)

$(CMD_TARGET): $(CMD_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CMD_OBJS) -o $(CMD_TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
