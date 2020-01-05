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
