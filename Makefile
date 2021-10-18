CXX=g++
CXXFLAGS= -std=c++17 -g -Wall -Wextra -Werror -Weffc++ -Wsign-conversion -pedantic-errors
CXXFLAGS += -lpthread
CXXFLAGS += -Idependencies
CXXFLAGS += -Ldependencies/libsdptransform
CXXFLAGS += -lsdptransform

SRC_DIR = .
OBJ_DIR = $(SRC_DIR)

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(SRC_DIR)/*.h*)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TARGET=$(shell basename ${PWD})

all: $(TARGET).out

$(TARGET).out: $(OBJ_FILES)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h%
	$(CXX) -c -o $@ $< $(CXXFLAGS)
clean:
	$(RM) *.out *.o