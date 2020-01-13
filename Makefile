# Use a dummy variable to run a shell command.
# This happens when the Makefile is read.
# i.e. before any make command is run.
_dummy := $(shell mkdir -p build/objects)

# GLAD_PATH := ./include/glad/%.c
# GLAD_SOURCES := $(wildcard ./include/glad/*.c)
# GLAD_OBJECTS := $(patsubst $(GLAD_PATH), ./build/%.o, $(GLAD_SOURCES))

# IMGUI_PATH := ./include/imgui/%.cpp
# IMGUI_SOURCES := $(wildcard ./include/imgui/*.cpp)
# IMGUI_OBJECTS := $(patsubst $(IMGUI_PATH), ./build/%.o, $(IMGUI_SOURCES))

# STB_PATH := ./include/stb/%.cpp
# STB_SOURCES := $(wildcard ./include/stb/*.cpp)
# STB_OBJECTS := $(patsubst $(STB_PATH), ./build/%.o, $(STB_SOURCES))

C_SOURCES := $(wildcard ./third_party_src/*.c)
CPP_SOURCES := $(wildcard ./third_party_src/*.cpp)

C_OBJECTS := $(patsubst ./third_party_src/%.c, ./build/objects/%.o, $(C_SOURCES))
CPP_OBJECTS := $(patsubst ./third_party_src/%.cpp, ./build/objects/%.o, $(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)

MUTINY_PREREQS := src/**/* $(OBJECTS)

CPPFLAGS := -std=c++17 -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include
LDLIBS := -lglfw -ldl
OPTIONS := $(CPPFLAGS) src/*.cpp $(OBJECTS) $(LDLIBS)

all: objects mutiny

clean:
	rm -rf build

objects: $(OBJECTS)
	@echo $(OBJECTS)

./build/objects/%.o: ./third_party_src/%.c
	g++ -isystem ./include -c $< -o $@

./build/objects/%.o: ./third_party_src/%.cpp
	g++ -isystem ./include -isystem ./include/imgui -c $< -o $@

mutiny: ./build/mutiny
	@echo ./build/mutiny

./build/mutiny: $(MUTINY_PREREQS)
	g++ $(OPTIONS) -o ./build/mutiny

debug: ./build/debug
	@echo ./build/debug

./build/debug: $(MUTINY_PREREQS)
	g++ -g3 $(OPTIONS) -o ./build/debug

# Glad
# glad: $(GLAD_OBJECTS)
	# @echo $(GLAD_OBJECTS)
# build/%.o: $(GLAD_PATH)
	# g++ -isystem ./include -c $< -o $@

# DearImgui
# imgui: $(IMGUI_OBJECTS)
	# @echo $(IMGUI_OBJECTS)
# build/%.o: $(IMGUI_PATH)
	# g++ -isystem ./include -c $< -o $@

# STB
# stb: $(STB_OBJECTS)
	# @echo $(STB_OBJECTS)
# build/%.o: $(STB_PATH)
	# g++ -isystem ./include -c $< -o $@

# Other Tasks
watch:
	find -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "Makefile" | entr -scr "make && ./build/mutiny"

min:
	g++ -g3 $(CPPFLAGS) -o minimal_example/minimal minimal_example/*.c* $(LDLIBS)

# * When program is segfaulting, easily generate and inspect core file *
#
# Ensures core files can be created,
# destroys any existing core file,
# creates the debug build,
# runs it to segfault and generate new core file,
# starts gdb with new core file.
core:
	ulimit -c unlimited && rm core; make debug && ./build/debug; gdb ./build/debug core

# * Shortcut to debug core file in new tmux window *
gdb:
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "make core" C-m

# * Exports my bookstack document as a GFM README.md file
readme:
	./bin/create_README.sh
