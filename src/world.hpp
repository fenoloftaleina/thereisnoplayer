#ifndef WORLD
#define WORLD
#pragma once

#include "buffer_object.hpp"
#include "nimate.hpp"
#include "models.hpp"
#include "common.hpp"
#include <bx/math.h>

#define fr(i, xs) for(int i = 0; i < xs.size(); ++i)

struct Spot
{
  int x;
  int y;
};

struct World
{
  std::vector<Spot> moving_spots;
  std::vector<Spot> moving_next_spots;
  std::vector<Spot> moving_cur_spots;
  std::vector<Spot> static_spots;
  std::vector<Spot> doors_spots;
  std::vector<Spot> winning_doors_spots;
  std::vector<Spot> editor_spot;

  std::vector<std::vector<Spot>> all_moving_spots;

  std::vector<bx::Vec3> moving_positions;
  std::vector<bx::Vec3> moving_colors;
  std::vector<bx::Vec3> static_positions;
  std::vector<bx::Vec3> static_colors;
  std::vector<bx::Vec3> doors_positions;
  std::vector<bx::Vec3> doors_colors;
  std::vector<bx::Vec3> winning_doors_positions;
  std::vector<bx::Vec3> winning_doors_colors;
  std::vector<bx::Vec3> editor_position;
  std::vector<bx::Vec3> editor_color;

  BufferObject moving_bo;
  BufferObject static_bo;
  BufferObject doors_bo;
  BufferObject winning_doors_bo;
  BufferObject editor_bo;


  BufferObject models_bo;


  Models models;
  Nimate nimate;

  bx::Vec3 moving_color = {0.85f, 0.2f, 0.32f};
  bx::Vec3 static_color = {0.0f, 99/255.0f, 115/255.0f};
  bx::Vec3 winning_doors_color = {0.5f, 0.5f, 0.5f};
  bx::Vec3 gate_colors[5] = {
    {0.5f, 0.2f, 0.4f},
    {0.6f, 0.8f, 0.4f},
    {0.3f, 0.9f, 0.8f},
    {0.2f, 0.3f, 0.8f},
    {0.1f, 0.6f, 1.0f},
  };
  bx::Vec3 editor_thing_color = {0.3f, 0.3f, 0.3f};

  void prepare();
  void init();
  void updateBuffers();

  void resolve(const Spot& move, const bool in_editor, const bool back, const bool reset);
  void update(const float dt);

  void draw(const bool in_editor);
  void destroy();


  static bool same(const Spot& s1, const Spot& s2);
  static void equals_sum(Spot& e, const Spot& s1, const Spot& s2);

  bool maybe_won();
  void maybe_make_move(const Spot& move);
  void maybe_doors();
  void execute_back();
  void execute_reset();
  void make_editor_move(const Spot& move);

  void setPositionsFromSpots(std::vector<bx::Vec3>& positions, const std::vector<Spot>& spots);
  void setColors(std::vector<bx::Vec3>& colors, const bx::Vec3& color);
  void writeCubesVertices(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors);
  void writeModelsVertices(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors, const int nth_model);

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(moving_spots, static_spots, doors_spots, winning_doors_spots);
  }

  int towards;
  int winning_count;
  bool made_move;
  bool travel;
  float animation_length;
  bool won;
  bool finished;
  std::vector<bx::Vec3> positions_temp;
  std::vector<bx::Vec3> colors_temp;
  std::vector<float> lengths_temp;
};

#endif
