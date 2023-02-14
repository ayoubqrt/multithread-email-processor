CXX      := -g++
CXXFLAGS := -std=c++17 -pthread -O2
BUILD    := ./build
BIN=./bin/
APP_DIR  := bin
TARGET   := program
INCLUDE  := -Iinclude/
SRC      :=  $(wildcard src/m*.cpp)

OBJECTS  := $(SRC:%.cpp=$(BUILD)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(BUILD)/%.o: %.cpp
    @mkdir -p $(@D)
    $(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
    @mkdir -p $(@D)
    $(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
    @mkdir -p $(APP_DIR)
    @mkdir -p $(BUILD)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
    -@rm -rvf $(BUILD)/*