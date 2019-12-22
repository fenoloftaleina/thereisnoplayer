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

  void add(std::vector<Spot>& spots, const Spot& spot);
  void remove(const Spot& spot);

  int find(const std::vector<Spot>& spots, const Spot& spot);
  bool remove_for(std::vector<Spot>& spots, const Spot& spot);

  int id;
};

#endif
