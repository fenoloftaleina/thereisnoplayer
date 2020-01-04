#ifndef NIMATE
#define NIMATE
#pragma once

#include "common.hpp"
#include <bx/math.h>
#include <vector>

struct BufferObject;
struct PosColorVertex;

struct Nimate
{
  std::vector<std::vector<bx::Vec3>> next_moving_positions;
  std::vector<std::vector<bx::Vec3>> next_moving_colors;

  std::vector<std::vector<PosColorVertex>> next_moving_vertices;

  std::vector<float> moving_positions_times;
  std::vector<std::vector<float>> next_moving_positions_lengths;

  std::vector<float> moving_colors_times;
  std::vector<std::vector<float>> next_moving_colors_lengths;

  std::vector<float> moving_vertices_times;
  std::vector<std::vector<float>> next_moving_vertices_lengths;

  void schedule(
      std::vector<std::vector<bx::Vec3>>& next_values,
      const std::vector<bx::Vec3>& goal,
      std::vector<std::vector<float>>& next_lengths,
      std::vector<float>& lengths,
      std::vector<float>& times
      );
  void schedule_vertices(
      const PosColorVertex* goal,
      const int goal_count,
      std::vector<std::vector<float>>& next_lengths,
      std::vector<float>& lengths,
      std::vector<float>& times
      );

  void run(
      const float dt,
      std::vector<bx::Vec3>& output_value,
      std::vector<std::vector<bx::Vec3>>& next_values,
      std::vector<float>& times,
      std::vector<std::vector<float>>& next_lenghts
      );

  void abort(
      std::vector<std::vector<bx::Vec3>>& next_values,
      std::vector<float>& times
      );

  void reset();

  float fraction;
  int finished_count;
};

#include "world.hpp"
#include "buffer_object.hpp"

#endif
