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
  std::vector<int>* models;
  std::vector<int>* flag1s;

  std::vector<bx::Vec3> next_positions;
  std::vector<bx::Vec3> next_colors;
  std::vector<int> next_models;
  std::vector<int> next_flag1s;

  std::vector<float> positions_from;
  std::vector<float> positions_to;
  std::vector<float> colors_from;
  std::vector<float> colors_to;
  std::vector<float> models_from;
  std::vector<float> models_to;
  std::vector<float> flag1s_from;
  std::vector<float> flag1s_to;

  std::vector<int> ids_positions;
  std::vector<int> ids_colors;
  std::vector<int> ids_models;
  std::vector<int> ids_flag1s;

  std::vector<int> updated_positions;
  std::vector<int> updated_colors;
  std::vector<int> updated_models;
  std::vector<int> updated_flag1s;

  void prepare(
      World* _world,
      BufferObject* _bo,
      std::vector<bx::Vec3>* _positions,
      std::vector<bx::Vec3>* _colors,
      std::vector<int>* _models,
      std::vector<int>* flag1s
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
  void schedule_model(
      const int id,
      const int nth_model,
      const float from,
      const float to
      );
  void schedule_flag1(
      const int id,
      const int value,
      const float from,
      const float to
      );
  void run(const float t, const bool update_anyway = false);

  void reset();


  bool needs_update;
  std::vector<bx::Vec3> positions_temp;
  std::vector<bx::Vec3> colors_temp;
  std::vector<int> models_temp;
  std::vector<bx::Vec3> froms_temp;
  std::vector<bx::Vec3> tos_temp;
};

#include "world.hpp"

#endif
