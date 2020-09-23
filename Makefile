#**********************************************************************
# Pre-Task Commands
#
# Use a dummy variable to run a shell command.
# This happens when the Makefile is read.
# i.e. before any make command is run.
#**********************************************************************
_create_objects_dir_dummy := $(shell mkdir -p build/objects)
_create_test_dir_dummy := $(shell mkdir -p build/test)

#**********************************************************************
# Variables
#**********************************************************************
SRC := src/*.cpp src/**/*.cpp

C_SOURCES := $(wildcard ./third_party_src/*.c)
CPP_SOURCES := $(wildcard ./third_party_src/*.cpp)

C_OBJECTS := $(patsubst ./third_party_src/%.c, ./build/objects/%.o, $(C_SOURCES))
CPP_OBJECTS := $(patsubst ./third_party_src/%.cpp, ./build/objects/%.o, $(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)
ARCHIVES := ./vendor/libnoise.a ./vendor/libmruby.a

MUTINY_PREREQS := src/* src/**/* $(OBJECTS) $(ARCHIVES)

CPPFLAGS := -std=c++17 -pthread -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include
LDLIBS := -lglfw -ldl -lstdc++fs
SHARED_OPTIONS := $(CPPFLAGS) $(SRC) $(OBJECTS) $(ARCHIVES) $(LDLIBS)
PRODUCTION_OPTIONS := -O3
DEBUG_OPTIONS := -g3 -O0

#**********************************************************************
# Tasks
#**********************************************************************
all: mutiny debug

clean:
	rm -rf build vendor/*.a


mutiny: ./build/mutiny
	@echo ./build/mutiny

./build/mutiny: $(MUTINY_PREREQS)
	g++ $(PRODUCTION_OPTIONS) $(SHARED_OPTIONS) -o ./build/mutiny

debug: ./build/debug
	@echo ./build/debug

./build/debug: $(MUTINY_PREREQS)
	g++ $(DEBUG_OPTIONS) $(SHARED_OPTIONS) -o ./build/debug


objects: $(OBJECTS)
	@echo $(OBJECTS)

./build/objects/%.o: ./third_party_src/%.c
	g++ -isystem ./include -c $< -o $@

./build/objects/%.o: ./third_party_src/%.cpp
	g++ -isystem ./include -isystem ./include/imgui -c $< -o $@


archives: $(ARCHIVES)
	@echo $(ARCHIVES)

#**********************************************************************
# Archives
#**********************************************************************
./vendor_src/mruby-2.1.1.zip:
	wget -O ./vendor_src/mruby-2.1.1.zip https://github.com/mruby/mruby/archive/2.1.1.zip

./vendor_src/mruby-2.1.1: ./vendor_src/mruby-2.1.1.zip
	unzip ./vendor_src/mruby-2.1.1.zip -d ./vendor_src/
	rm ./vendor_src/mruby-2.1.1/build_config.rb
	cd ./vendor_src/mruby-2.1.1 && ln -s ../../mruby_build_config.rb build_config.rb

./vendor/libmruby.a: ./mruby_build_config.rb
	cd ./vendor_src/mruby-2.1.1 && rake
	cp vendor_src/mruby-2.1.1/build/host/lib/libmruby.a vendor/libmruby.a


./vendor_src/libnoise:
	cd ./vendor_src && git clone git@github.com:qknight/libnoise.git

./vendor/libnoise.a:
	cd ./vendor_src/libnoise && mkdir -p build && cd build && cmake .. && make
	cp vendor_src/libnoise/build/src/libnoise.a vendor/libnoise.a

#**********************************************************************
# Testing
#**********************************************************************
TEST_SOURCES := $(wildcard ./test/*.cpp)
TEST_NAMES := $(patsubst ./test/%.cpp, %, $(TEST_SOURCES))

test: $(TEST_NAMES)
	@echo $(TEST_NAMES)

define test_template
$(1): ./build/test/$(1)
	./build/test/$(1)

$(1)-gdb: ./build/test/$(1)
	gdb ./build/test/$(1)
endef

$(foreach test_name, $(TEST_NAMES), $(eval $(call test_template,$(test_name))))

./build/test/%: ./test/%.cpp $(MUTINY_PREREQS)
	g++ -g3 $(CPPFLAGS) -I $(ASDF) $< $(OBJECTS) $(ARCHIVES) $(LDLIBS) -o $@

watch_tests:
	find test -type f | entr -scr "make test"

#**********************************************************************
# Debugging
#**********************************************************************
# When program is segfaulting, easily generate and inspect core file
#
#     1. Ensures core files can be created
#     2. Destroys any existing core file
#     3. Creates the debug build
#     4. Runs it to segfault and generate new core file
#     5. Starts gdb with new core file
#
core: debug
	ulimit -c unlimited && rm core; make debug && ./build/debug; gdb ./build/debug core

# Start new tmux window debugging core file
gdbcore: debug
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "make core" C-m

# Start new tmux window debugging debug build
gdb: debug
	tmux new-window -n gdb
	tmux send-keys -t gdb.0 "gdb ./build/debug" C-m

#**********************************************************************
# Other Tasks
#**********************************************************************
watch:
	find src scripts -type f | entr -scr "make mutiny && ./build/mutiny"
