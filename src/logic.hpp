#ifndef LOGIC
#define LOGIC
#pragma once

#include "objs.hpp"

struct Logic
{
  Objs objs;

  bool were_collisions;
  bool kids_collisions;
  bx::Vec3 kids_moves[4][4] = {
    {
      {1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, -1.0f},
      {-1.0f, 0.0f, 0.0f},
    },
    {
      {0.0f, 0.0f, 1.0f},
      {-1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f},
    },
    {
      {0.0f, 0.0f, -1.0f},
      {1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f},
    },
    {
      {-1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f},
    },
  };
  int winning_count;

  bool run(const bx::Vec3& cur_pos, const bool in_editor);
  void editor(const bx::Vec3& cur_pos);
  bool collidesWith(const Cube& cube, const std::vector<Cube>& cubes, const int start_j, const bool with_moving);
  bool collidesWith(const std::vector<Cube>& cubes1, const std::vector<Cube>& cubes2, const bool same, const bool with_moving);
  void maybeDoors(Cube& cube);
  bool movement(const bx::Vec3& cur_pos);
  bool tryKidMovement(Cube& cube);
  bool kidsMovement();
};

#endif
