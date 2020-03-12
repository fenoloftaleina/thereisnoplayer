#ifndef EDITOR
#define EDITOR
#pragma once

#include <stdio.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <vector>

#include "world.hpp"


struct Editor
{
  World* world;

  void add(std::vector<Spot>& spots, const Spot& spot, BufferObject& bo);
  void add(std::vector<Spot>& spots, const Spot& spot, BufferObject& bo, std::vector<int>& list, int id);
  void remove(const Spot& spot);

  int find(const std::vector<Spot>& spots, const Spot& spot);
  bool remove_for(std::vector<Spot>& spots, const Spot& spot);

  void next_mapping(int i, BufferObject& bo, std::vector<int>& list, int possible_mappings_count);

  int id;
};

#endif
