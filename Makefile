all:
CXX = clang++
CXXFLAGS = -MMD -MP -Wall -Wexceptions -std=c++11 -g `sdl2-config --cflags` -Ibgfx/include -Ibx/include -Ibimg/include -Iassimp/include
# LDFLAGS = `sdl2-config --libs` bgfx/.build/osx64_clang/bin/libbgfx-shared-libRelease.dylib
LDFLAGS = `sdl2-config --libs` bgfx/.build/osx64_clang/bin/libbgfxDebug.a bgfx/.build/osx64_clang/bin/libbxDebug.a bgfx/.build/osx64_clang/bin/libbimgDebug.a -Wl,-rpath,assimp/lib/ -Lassimp/lib/ -lassimp -framework Metal -framework Cocoa -lc++ -framework Carbon -framework QuartzCore -framework OpenGL -framework IOKit
SHADERS_PLATFORM = --platform osx -p metal
TARGET = main

SOURCES = $(wildcard src/*.cpp)
SHADERS = $(wildcard src/shaders/f_*.c) $(wildcard src/shaders/v_*.c)

all: $(TARGET) shaders

$(TARGET): $(SOURCES:src/%.cpp=bin/%.o)
	$(CXX) $(LDFLAGS) $^ -o $(TARGET)

bin/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

-include bin/*.d

clean:
	@rm -f $(TARGET) bin/*.o bin/*.d bin/*.bin

shaders: $(SHADERS:src/shaders/%.c=bin/%.bin)

bin/f_%.bin: src/shaders/f_%.c
	bgfx/.build/osx64_clang/bin/shadercDebug -f $< -o $@ -i bgfx/src $(SHADERS_PLATFORM) --type fragment

bin/v_%.bin: src/shaders/v_%.c
	bgfx/.build/osx64_clang/bin/shadercDebug -f $< -o $@ -i bgfx/src $(SHADERS_PLATFORM) --type vertex
