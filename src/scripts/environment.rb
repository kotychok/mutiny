# TODO Constants should be defined from C++ where they are defined in C++
# instead of copy-pasted.
CHUNK_SIZE = 32;
CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
CHUNK_SIZE_HALVED = CHUNK_SIZE / 2;
CHUNK_SIZE_QUARTERED = CHUNK_SIZE / 4;

require "./src/scripts/chunk_generator"
