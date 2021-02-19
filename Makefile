#**********************************************************************
# Variables
#**********************************************************************
SRC := src/*.cpp src/**/*.cpp

C_SOURCES := $(wildcard ./third_party_src/*.c)
CPP_SOURCES := $(wildcard ./third_party_src/*.cpp)

C_OBJECTS := $(patsubst ./third_party_src/%.c, ./build/objects/%.o, $(C_SOURCES))
CPP_OBJECTS := $(patsubst ./third_party_src/%.cpp, ./build/objects/%.o, $(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)

CPPFLAGS = -std=c++17 -pthread -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -isystem ./include
LDLIBS := -lglfw -ldl -lstdc++fs

# vendor
VENDOR_DIR := vendor

# libnoise
LIBNOISE_ARCHIVE_PATH := $(VENDOR_DIR)/libnoise/build/src/libnoise.a
ARCHIVES += $(LIBNOISE_ARCHIVE_PATH)
LIBNOISE_HEADERS_PATH := $(VENDOR_DIR)/libnoise/src
CPPFLAGS += -isystem $(LIBNOISE_HEADERS_PATH)

# mruby
MRUBY_VERSION := 2.1.1
MRUBY_ARCHIVE_PATH := $(VENDOR_DIR)/mruby-$(MRUBY_VERSION)/build/host/lib/libmruby.a
ARCHIVES += $(MRUBY_ARCHIVE_PATH)
MRUBY_HEADERS_PATH := $(VENDOR_DIR)/mruby-$(MRUBY_VERSION)/include
CPPFLAGS += -isystem $(MRUBY_HEADERS_PATH)

MUTINY_PREREQS = src/* src/**/* $(OBJECTS) $(ARCHIVES)
SHARED_OPTIONS = $(CPPFLAGS) $(SRC) $(OBJECTS) $(ARCHIVES) $(LDLIBS)
PRODUCTION_OPTIONS := -O3
DEBUG_OPTIONS := -g3 -O0

#**********************************************************************
# Tasks
#**********************************************************************
all: mutiny debug

clean:
	rm -rf build


mutiny: ./build/mutiny
	@echo ./build/mutiny

./build/mutiny: $(MUTINY_PREREQS)
	echo $(ARCHIVES)
	g++ $(PRODUCTION_OPTIONS) $(SHARED_OPTIONS) -o ./build/mutiny

debug: ./build/debug
	@echo ./build/debug

./build/debug: $(MUTINY_PREREQS)
	g++ $(DEBUG_OPTIONS) $(SHARED_OPTIONS) -o ./build/debug


objects: $(OBJECTS)
	@echo $(OBJECTS)

./build/objects/%.o: ./third_party_src/%.c | build/objects
	g++ -isystem ./include -c $< -o $@

./build/objects/%.o: ./third_party_src/%.cpp | build/objects
	g++ -isystem ./include -isystem ./include/imgui -c $< -o $@

build/objects: | build
	mkdir -p $@

build:
	mkdir -p $@

archives: $(ARCHIVES)
	@echo $(ARCHIVES)

#**********************************************************************
# Vendor
#**********************************************************************

vendor_clean:
	rm -rf $(VENDOR_DIR)

$(VENDOR_DIR):
	mkdir -p $@

#**********************************
# libnoise
#**********************************
libnoise.a: $(LIBNOISE_ARCHIVE_PATH)

.PHONY: libnoise.a

$(LIBNOISE_ARCHIVE_PATH): | $(VENDOR_DIR)/libnoise
	cd $(VENDOR_DIR)/libnoise && mkdir -p build && cd build && cmake .. && make

$(VENDOR_DIR)/libnoise: | $(VENDOR_DIR)
	cd $(VENDOR_DIR) && git clone git@github.com:qknight/libnoise.git

#**********************************
# mruby
#**********************************
libmruby.a: $(MRUBY_ARCHIVE_PATH)

.PHONY: libmruby.a

$(MRUBY_ARCHIVE_PATH): mruby_build_config.rb | $(VENDOR_DIR)/mruby-$(MRUBY_VERSION)
	cd $(VENDOR_DIR)/mruby-$(MRUBY_VERSION) && rake

$(VENDOR_DIR)/mruby-$(MRUBY_VERSION): $(VENDOR_DIR)/mruby-$(MRUBY_VERSION).zip
	unzip $< -d $(VENDOR_DIR)/
	rm $(VENDOR_DIR)/mruby-$(MRUBY_VERSION)/build_config.rb
	cd $(VENDOR_DIR)/mruby-$(MRUBY_VERSION) && ln -s ../../mruby_build_config.rb build_config.rb

$(VENDOR_DIR)/mruby-$(MRUBY_VERSION).zip: | $(VENDOR_DIR)
	wget -O $@ https://github.com/mruby/mruby/archive/$(MRUBY_VERSION).zip

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

build/test/%: test/%.cpp $(MUTINY_PREREQS) | build/test
	g++ -g3 $(CPPFLAGS) -I $(ASDF) $< $(OBJECTS) $(ARCHIVES) $(LDLIBS) -o $@

build/test:
	mkdir -p $@

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
SHELL := /bin/bash # Make `source` command available.
ENV := source env.sh # Set environment variables for BookStack auth
BOOKSTACK_CLI_CMD=RUBYOPT=-W:no-experimental bundle exec bookstack-cli

watch:
	find src scripts -type f | entr -scr "make mutiny && ./build/mutiny"

README.md:
	$(ENV) && $(BOOKSTACK_CLI_CMD) export --output_file=README.md --output_dir=creating-a-voxel-renderer-from-scratch chapter hot-creating-a-voxel-renderer-from-scratch

.PHONY: watch README.md
