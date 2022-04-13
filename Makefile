CC := g++
FLAGS := -pedantic -Wall
NAME := bza_platypus
SRC_DIR := scr/
BUILD_DIR := build/
SRCS := $(wildcard src/*.cpp)
OBJECTS := $(addprefix build/,$(notdir $(SRCS:.cpp=.o)))
LIBS := `pkg-config --libs powercap`
PACKFILE := xnemet04.zip

.PHONY: build clean pack

all: build

run: build
	@echo "---COMPILATION FINISHED---"
	@sudo ./bza_platypus

build: $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $^ $(LIBS)

build/%.o: src/%.cpp
	$(CC) $(FLAGS) -o $@ -c $^ $(LIBS)

pack:
	zip $(PACKFILE) $(SRCS) $(SRCS:.cpp=.hpp) Makefile

clean:
	rm -f $(NAME) $(OBJECTS) $(PACKFILE)
