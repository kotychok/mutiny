# Use a dummy variable to run a shell command.
# This happens when the Makefile is read.
# i.e. before any make command is run.
_dummy := $(shell mkdir -p build/objects)
_dummy2 := $(shell mkdir -p build/test)

C_SOURCES := $(wildcard ./third_party_src/*.c)
CPP_SOURCES := $(wildcard ./third_party_src/*.cpp)

C_OBJECTS := $(patsubst ./third_party_src/%.c, ./build/objects/%.o, $(C_SOURCES))
CPP_OBJECTS := $(patsubst ./third_party_src/%.cpp, ./build/objects/%.o, $(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)

MUTINY_PREREQS := src/* src/**/* $(OBJECTS)

CPPFLAGS := -std=c++17 -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include
LDLIBS := -lglfw -ldl -lstdc++fs
OPTIONS := $(CPPFLAGS) src/*.cpp src/lib/*.cpp $(OBJECTS) $(LDLIBS)

all: objects mutiny test

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
	g++ -g3 -O0 $(OPTIONS) -o ./build/debug

# Testing
TEST_SOURCES := $(wildcard ./test/*.cpp)
TEST_NAMES := $(patsubst ./test/%.cpp, %, $(TEST_SOURCES))

test: $(TEST_NAMES)
	@echo $(TEST_NAMES)

define test_template
$(1): ./build/test/$(1)
	./build/test/$(1) --report_level=detailed
endef

$(foreach test_name, $(TEST_NAMES), $(eval $(call test_template,$(test_name))))

./build/test/%: ./test/%.cpp $(MUTINY_PREREQS)
	g++ -g3 $(CPPFLAGS) -I src/lib src/lib/*.cpp $< $(OBJECTS) $(LDLIBS) -o $@

watch_tests:
	find test -type f | entr -scr "make test"

# Other Tasks
watch:
	find src -type f | entr -scr "make mutiny && ./build/mutiny"

min:
	g++ -g3 $(CPPFLAGS) -o minimal_example/minimal minimal_example/*.c* $(LDLIBS)

# * When program is segfaulting, easily generate and inspect core file *
#
# Ensures core files can be created,
# destroys any existing core file,
# creates the debug build,
# runs it to segfault and generate new core file,
# starts gdb with new core file.
core: debug
	ulimit -c unlimited && rm core; make debug && ./build/debug; gdb ./build/debug core

# * Shortcut to debug core file in new tmux window *
gdbcore: debug
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "make core" C-m

gdb: debug
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "gdb ./build/debug" C-m

# * Exports my bookstack document as a GFM README.md file
readme:
	./bin/create_README
