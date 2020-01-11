CPPFLAGS := -std=c++17 -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include/
LDLIBS := -lglfw -ldl
OPTIONS := $(CPPFLAGS) -o build/mutiny src/*.c src/*.cpp $(LDLIBS)

mutiny: src/main.cpp
	mkdir -p build
	g++ $(OPTIONS)

debug: src/main.cpp
	mkdir -p build
	g++ -g3 $(OPTIONS)

clean:
	rm -rf build

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
	ulimit -c unlimited && rm core; make debug && ./build/mutiny; gdb ./build/mutiny core

# * Shortcut to debug core file in new tmux window *
gdb:
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "make core" C-m
