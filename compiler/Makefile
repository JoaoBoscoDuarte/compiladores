CXX := g++
INCLUDE_DIR := include
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

EV_TARGET := $(BIN_DIR)/ev
EV_OBJS := $(OBJ_DIR)/ev.o $(OBJ_DIR)/ev_lexer.o $(OBJ_DIR)/ev_parser.o $(OBJ_DIR)/ev_semantic.o $(OBJ_DIR)/ev_codegen.o

.PHONY: all clean

all: $(EV_TARGET)

$(EV_TARGET): $(EV_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(EV_OBJS) -o $(EV_TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
