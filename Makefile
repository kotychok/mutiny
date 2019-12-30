mutiny: src/main.cpp
	mkdir -p build
	g++ -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include/ -o build/mutiny src/glad.c src/*.cpp -lglfw -ldl

clean:
	rm -rf build
