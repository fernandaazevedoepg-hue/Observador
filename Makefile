# Observador - Makefile (Linux)
#
# Build assumes raylib static library is at ./lib/libraylib.a
# and headers are in ./include (a local copy of raylib.h is included).
#
# If you get linker errors, install the raylib deps:
#   sudo apt install build-essential libx11-dev libxrandr-dev libxi-dev \
#                    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev

CXX      := g++
TARGET   := Observador
BIN_DIR  := bin
OBJ_DIR  := obj
SRC_DIR  := src
INC_DIR  := include
LIB_DIR  := lib

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -I$(INC_DIR)
LDLIBS   := -L$(LIB_DIR) -lraylib -lm -ldl -lpthread -lX11

.PHONY: all run clean rebuild help

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: all
	./$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/$(TARGET)

rebuild: clean all

help:
	@echo "Targets:"
	@echo "  make        - compila"
	@echo "  make run    - compila e executa"
	@echo "  make clean  - limpa"
	@echo "  make rebuild- recompila do zero"
