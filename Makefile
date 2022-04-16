CC := g++
FLAGS := -pedantic -Wall
NAME := bza_platypus
SRC_DIR := scr/
BUILD_DIR := build/
SRCS := $(wildcard src/*.cpp)
OBJECTS := $(addprefix build/,$(notdir $(SRCS:.cpp=.o)))
LIBS := `pkg-config --libs powercap`
PACKFILE := xnemet04.zip

# Python
VENV_PATH := .venv

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

# Python
venv:
	python3 -m venv $(VENV_PATH)

pip-install:
# TODO this doesn't work
ifeq ($(shell pip -V | grep $(VENV_PATH)), 0)
	python3 -m pip install -r requirements.txt
else
	@echo "Possibly not in venv. Run 'make venv' and then 'source $(VENV_PATH)/bin/activate'."
endif
