#ifndef NIMATE
#define NIMATE
#pragma once

#include "common.hpp"
#include "buffer_object.hpp"
#include <bx/math.h>
#include <vector>

struct World;

struct Nimate
{
  World* world;
  BufferObject* bo;
  std::vector<bx::Vec3>* positions;
  std::vector<bx::Vec3>* colors;

  std::vector<bx::Vec3> next_positions;
  std::vector<bx::Vec3> next_colors;

  std::vector<float> positions_from;
  std::vector<float> positions_to;
  std::vector<float> colors_from;
  std::vector<float> colors_to;

  std::vector<int> ids_positions;
  std::vector<int> ids_colors;

  std::vector<bool> updated_positions;
  std::vector<bool> updated_colors;

  void prepare(
      World* _world,
      BufferObject* _bo,
      std::vector<bx::Vec3>* _positions,
      std::vector<bx::Vec3>* _colors
      );
  void init();

  void schedule_position(
      const int id,
      const bx::Vec3& position,
      const float from,
      const float to
      );
  void schedule_color(
      const int id,
      const bx::Vec3& color,
      const float from,
      const float to
      );
  void run(const float t, const bool update_anyway = false);

  void reset();

  float fraction;
  int finished_count;

  int positions_size;
  int colors_size;
  int new_positions_size;
  int new_colors_size;
  bool needs_update;
  bool bool_temp;
  std::vector<bx::Vec3> positions_temp;
  std::vector<bx::Vec3> colors_temp;
  std::vector<bx::Vec3> froms_temp;
  std::vector<bx::Vec3> tos_temp;
};

#include "world.hpp"

#endif
