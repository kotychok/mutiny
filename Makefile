OPTIONS := -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include/ -o build/mutiny src/*.c src/*.cpp -lglfw -ldl

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
