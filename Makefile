OS := $(shell uname -s)

ifeq ($(OS),Linux)
    CXX = g++
    CC = gcc
    LIBS = 
    ARGS = -I"libs/include"
endif

ifeq ($(OS),Windows_NT)
    CXX = g++
    CC = gcc
    LIBS = 
    ARGS = -lws2_32 -I"libs/include" -L"libs/lib" -lglfw3 -lopengl32 -lgdi32 -limgui
endif

SRC_DIR := src
OBJ_DIR := build
BIN := bin/encoder

SOURCES_CPP := $(shell find $(SRC_DIR) -type f -name "*.cpp")
SOURCES_C := $(shell find $(SRC_DIR) -type f -name "*.c")
SOURCES := $(SOURCES_CPP) $(SOURCES_C)

OBJECTS_CPP := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SOURCES_CPP:.cpp=.o))
OBJECTS_C := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SOURCES_C:.c=.o))
OBJECTS := $(OBJECTS_CPP) $(OBJECTS_C)

$(shell mkdir -p $(OBJ_DIR))

$(BIN): $(OBJECTS)
	$(CXX) $^ -o $@ $(ARGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(ARGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ARGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)
