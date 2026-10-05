CXX := g++
INCLUDE_DIR := include
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

CI_TARGET := $(BIN_DIR)/compilerci
CI_OBJS := $(OBJ_DIR)/compilerci.o $(OBJ_DIR)/parser.o $(OBJ_DIR)/codegen.o

EC1_TARGET := $(BIN_DIR)/ec1
EC1_OBJS := $(OBJ_DIR)/ec1.o $(OBJ_DIR)/ec1_lexer.o $(OBJ_DIR)/ec1_parser.o

.PHONY: all clean

all: $(CI_TARGET) $(EC1_TARGET)

$(CI_TARGET): $(CI_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CI_OBJS) -o $(CI_TARGET)

$(EC1_TARGET): $(EC1_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(EC1_OBJS) -o $(EC1_TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
