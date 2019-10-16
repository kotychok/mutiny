mutiny: src/main.cpp
	mkdir -p build
	g++ -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -I ./include/ -o build/mutiny src/glad.c src/main.cpp -lglfw -ldl

clean:
	rm -rf build
