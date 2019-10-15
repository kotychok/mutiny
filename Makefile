mutiny: src/main.cpp
	mkdir -p build
	g++ -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -o build/mutiny src/main.cpp

clean:
	rm -rf build
