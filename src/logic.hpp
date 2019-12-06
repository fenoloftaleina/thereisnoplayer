#ifndef LOGIC
#define LOGIC
#pragma once

#include "objs.hpp"

struct Logic
{
  Objs* objs;
  Level* level;

  bool were_collisions;
  int winning_count;

  bool run(const bx::Vec3& cur_pos, const bool in_editor, const bool back = false, const float dt = 0.1f);
  void editor(const bx::Vec3& cur_pos);
  bool collidesWith(const Cube& cube, const std::vector<Cube>& cubes, const int start_j, const bool with_moving);
  bool collidesWith(const std::vector<Cube>& cubes1, const std::vector<Cube>& cubes2, const bool same, const bool with_moving);
  void maybeDoors(Cube& cube);
  bool movement(const bx::Vec3& cur_pos);
};

#endif
