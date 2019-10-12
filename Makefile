all:
CXX = clang++
CXXFLAGS = -MMD -MP -Wall -std=c++11 -g `sdl2-config --cflags` -Ibgfx/include -Ibx/include -Ibimg/include
# LDFLAGS = `sdl2-config --libs` bgfx/.build/osx64_clang/bin/libbgfx-shared-libRelease.dylib
LDFLAGS = `sdl2-config --libs` bgfx/.build/osx64_clang/bin/libbgfxDebug.a bgfx/.build/osx64_clang/bin/libbxDebug.a bgfx/.build/osx64_clang/bin/libbimgDebug.a -framework Metal -framework Cocoa -lc++ -framework Carbon -framework QuartzCore -framework OpenGL -framework IOKit
TARGET = main

SOURCES = $(wildcard src/*.cpp)
SHADERS = $(wildcard src/shaders/f_*.sc) $(wildcard src/shaders/v_*.sc)

all: $(TARGET) shaders

$(TARGET): $(SOURCES:src/%.cpp=bin/%.o)
	$(CXX) $(LDFLAGS) $^ -o $(TARGET)

bin/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

-include bin/*.d

clean:
	@rm -f $(TARGET) bin/*.o bin/*.d bin/*.bin

shaders: $(SHADERS:src/shaders/%.sc=bin/%.bin)

bin/f_%.bin: src/shaders/f_%.sc
	bgfx/.build/osx64_clang/bin/shadercDebug -f $< -o $@ -i bgfx/src --platform osx -p metal --type fragment

bin/v_%.bin: src/shaders/v_%.sc
	bgfx/.build/osx64_clang/bin/shadercDebug -f $< -o $@ -i bgfx/src --platform osx -p metal --type vertex
