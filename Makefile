GLM = lib/glm
SDL2 = lib/SDL2-2.0.3/x86_64-w64-mingw32
SDL2_32 = $(SDL2)/../i686-w64-mingw32
GLEW = lib/glew-1.11.0
BULLET = lib/bullet3

LIB_PATHS = -L$(GLEW)/lib -L$(SDL2)/lib -L$(BULLET)/lib
LIB_PATHS_32 = -L$(GLEW)/lib32 -L$(SDL2_32)/lib -L$(BULLET)/lib32
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lBulletDynamics -lBulletCollision -lLinearMath
LIB_INC_PATHS = -I$(GLEW)/include -I$(SDL2)/include -I$(GLM) -I$(BULLET)/src
LIB_INC_PATHS_32 = -I$(GLEW)/include -I$(SDL2_32)/include -I$(GLM) -I$(BULLET)/src
FLAGS = -Wall

LIBRARY_NAME = Module

FULL_NAME = lib$(LIBRARY_NAME).a


#Debug mode
FLAGS += -g -DDEBUG -ggdb 
PATH64 = build/debug/64bit
PATH32 = build/debug/32bit

#Release mode
#FLAGS += -DNDEBUG -mwindows 
#PATH64 = build/release/64bit
#PATH32 = build/release/32bit

SRC = $(wildcard src/*.cpp)
OBJ = $(notdir $(SRC:.cpp=.o))
OBJ_PATHS64 = $(addprefix $(PATH64)/,$(OBJ))
OBJ_PATHS32 = $(addprefix $(PATH32)/,$(OBJ))

ifeq ($(OS), Windows_NT)
	PATH64 := $(addsuffix /windows/mingw, $(PATH64))
	PATH32 := $(addsuffix /windows/mingw, $(PATH32))
else
	PATH64 := $(addsuffix /linux, $(PATH64))
	PATH32 := $(addsuffix /linux, $(PATH32))	
endif

all: 64bit 32bit

64bit: $(PATH64)/$(FULL_NAME)
32bit: $(PATH32)/$(FULL_NAME)
test: 64bit
	g++ main_test.cpp -Iinclude -L$(PATH64) -l$(LIBRARY_NAME) -o main_test
	
clean:
	rm -f $(PATH64)/*
	rm -f $(PATH32)/*

$(PATH64):
	mkdir -p $(PATH64)
	
$(PATH32):
	mkdir -p $(PATH32)
	
$(PATH64)/%.o : src/%.cpp
	g++ $< -c -Iinclude $(FLAGS) -o $@
	
$(PATH32)/%.o : src/%.cpp
	i686-w64-mingw32-g++ $< -c -Iinclude $(FLAGS) -o $@

$(PATH64)/$(FULL_NAME): $(OBJ_PATHS64)
	ar rcs $(PATH64)/$(FULL_NAME) $(PATH64)/*.o
#	g++ $(FLAGS) $(SRC) $(LIB_PATHS) $(LIB_INC_PATHS) $(LIBS) -o $@
	
$(PATH32)/$(FULL_NAME): $(OBJ_PATHS32)
	ar rcs $(PATH32)/$(FULL_NAME) $(PATH32)/*.o
#	i686-w64-mingw32-g++ $(FLAGS) $(SRC) $(LIB_PATHS_32) $(LIB_INC_PATHS_32) $(LIBS) -o $@

$(OBJ_PATHS64) : | $(PATH64)
$(OBJ_PATHS32) : | $(PATH32)