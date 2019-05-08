EXE = mesh-generator

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib/matplotlib-cpp-master

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -ggdb3 -Wall -Werror -pedantic -std=c++11
LDFLAGS += -Llib
LDLIBS += -lm

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	g++ $(LDFLAGS) $(LDLIBS) -o $@ $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
	$(RM) test/*.msh test/*.log
