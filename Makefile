CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -g $(shell pkg-config --cflags glib-2.0 libmodbus)

SRC_MAIN_DIR       := src
SRC_UTIL_DIR       := src/util
SRC_CONTROL_DIR    := src/control
SRC_GUI_DIR        := src/gui
SRC_CFGFILE_DIR    := src/config-file
SRC_STRUCT_DIR     := src/struct
SRC_CAL_DIR        := src/cal
SRC_TESTVALUES_DIR := src/test-values

INC_MAIN_DIR       := src/inc
INC_UTIL_DIR       := src/util
INC_CONTROL_DIR    := src/control
INC_GUI_DIR        := src/gui
INC_CFGFILE_DIR    := src/config-file
INC_STRUCT_DIR     := src/struct
INC_CAL_DIR        := src/cal
INC_TESTVALUES_DIR := src/test-values

BIN		:= bin
#leaving this stuff here because I may just get froggy and jump on this
#SRC		:= $(wildcard $(SRC_CAL_DIR)/*.cpp) \
#		   $(wildcard $(SRC_GUI_DIR)/*.c) \
#		   $(wildcard $(SRC_CONTROL_DIR)/*.cpp) \
#		   $(wildcard $(SRC_STRUCT_DIR)/*.cpp) \
#		   $(wildcard $(SRC_CFGFILE_DIR)/*.cpp) \
#		   $(wildcard $(SRC_UTIL_DIR)/*.cpp) \
#		   $(wildcard $(SRC_MAIN_DIR)/*.cpp)
SRC		:= $(wildcard $(SRC_MAIN_DIR)/*.cpp)
#INCLUDE :=  $(INC_CAL_DIR) \
#			$(INC_GUI_DIR) \
#			$(INC_CONTROL_DIR) \
#			$(INC_STRUCT_DIR) \
#			$(INC_CFGFILE_DIR) \
#			$(INC_UTIL_DIR) \
#			$(INC_MAIN_DIR)
INCLUDE :=  $(INC_MAIN_DIR)
INC_PARAMS = $(foreach d, $(INCLUDE), -I$d)
LIB		:=

LIBRARIES	:= $(shell pkg-config --libs glib-2.0 libmodbus)
EXECUTABLE	:= main

all: $(BIN)/$(EXECUTABLE)

run: clean all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)
	$(CXX) $(CXX_FLAGS) $(INC_PARAMS) $^ -o $@ $(LIBRARIES)
#	$(CXX) $(CXX_FLAGS) $(INC_PARAMS) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	install -d $(BIN)
	-rm -f $(BIN)/*
