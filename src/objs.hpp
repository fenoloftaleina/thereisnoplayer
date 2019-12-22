#ifndef OBJS
#define OBJS
#pragma once

#include "buffer_object.hpp"
#include "common.hpp"
#include <bx/math.h>

struct Cube {
  bx::Vec3 pos {0.0f, 0.0f, 0.0f};
  bx::Vec3 col {0.7f, 0.7f, 0.7f};

  bx::Vec3 spot {0, 0, 0};
  bx::Vec3 next_spot {0, 0, 0};
  bx::Vec3 cur_spot {0, 0, 0};

  float anim_fraction = 1.0f;
  bool on_door = false;
  bx::Vec3 on_door_col {0.0f, 0.0f, 0.0f};

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(spot.x, spot.y, spot.z);
  }
};

struct Door
{
  Cube cube;
  int towards;
  bx::Vec3 collision_face_normal;

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(cube, towards, collision_face_normal.x, collision_face_normal.y, collision_face_normal.z);
  }
};

struct Level
{
  std::vector<Cube> moving_cubes;
  std::vector<Cube> static_cubes;
  std::vector<Door> doors;
  std::vector<Door> winning_doors;

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(moving_cubes, static_cubes, doors, winning_doors);
  }
};

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
  Spot editor_spot {5, 5};

  std::vector<bx::Vec3> moving_positions;
  std::vector<bx::Vec3> moving_colors;
  std::vector<bx::Vec3> static_positions;
  std::vector<bx::Vec3> static_colors;
  std::vector<bx::Vec3> doors_positions;
  std::vector<bx::Vec3> doors_colors;
  std::vector<bx::Vec3> winning_doors_positions;
  std::vector<bx::Vec3> winning_doors_colors;
  bx::Vec3 editor_position;
  bx::Vec3 editor_color = {0.3f, 0.3f, 0.3f};

  BufferObject moving_bo;
  BufferObject static_bo;
  BufferObject doors_bo;
  BufferObject winning_doors_bo;
  BufferObject editor_bo;

  bx::Vec3 moving_color = {0.85f, 0.2f, 0.32f};
  bx::Vec3 static_color = {0.0f, 99/255.0f, 115/255.0f};
  bx::Vec3 doors_color = {0.1f, 99/255.0f, 15/255.0f};
  bx::Vec3 winning_doors_color = {0.5f, 0.5f, 0.5f};
  bx::Vec3 gate_colors[5] = {
    {0.5f, 0.2f, 0.4f},
    {0.6f, 0.8f, 0.4f},
    {0.3f, 0.9f, 0.8f},
    {0.2f, 0.3f, 0.8f},
    {0.1f, 0.6f, 1.0f},
  };

  void prepare();
  void init();
  void setPositionsFromSpots(std::vector<bx::Vec3>& positions, const std::vector<Spot>& spots);
  void setColors(std::vector<bx::Vec3>& colors, const bx::Vec3& color);
  void writeCubesVertices(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors);
  void draw(const bool in_editor);
  void destroy();

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(moving_spots, static_spots, doors_spots, winning_doors_spots);
  }
};

struct Objs
{
  BufferObject moving_bo;
  BufferObject static_bo;
  BufferObject doors_bo;
  BufferObject winning_doors_bo;
  BufferObject editor_bo;

  Cube editor_cube;
  Level* level;
  std::vector<std::vector<Cube>> previous_moving_cubes;

  const int cubes_in_memory_count = 300;
  const int doors_in_memory_count = 30;

  bx::Vec3 moving_cubes_color = {0.85f, 0.2f, 0.32f};
  bx::Vec3 kids_cubes_color = {0.85f, 0.3f, 0.52f};
  bx::Vec3 static_cubes_color = {0.0f, 99/255.0f, 115/255.0f};
  bx::Vec3 editor_cube_color = {0.3f, 0.3f, 0.3f};
  bx::Vec3 doors_color = {0.1f, 99/255.0f, 15/255.0f};
  bx::Vec3 winning_doors_color = {0.5f, 0.5f, 0.5f};
  bx::Vec3 gate_colors[5] = {
    {0.5f, 0.2f, 0.4f},
    {0.6f, 0.8f, 0.4f},
    {0.3f, 0.9f, 0.8f},
    {0.2f, 0.3f, 0.8f},
    {0.1f, 0.6f, 1.0f},
  };
  float fraction;

  void preInit();
  void init();
  void initBos();

  void initCube(const int i, Cube& cube, BufferObject& bo);
  void initCube(const int i, Cube& cube, bx::Vec3& col, BufferObject& bo);
  void initCubes(const int i, std::vector<Cube>& cubes, BufferObject& bo);
  void initCubes(const int i, std::vector<Cube>& cubes, bx::Vec3& col, BufferObject& bo);
  void initDoorsCubes(const int i, std::vector<Door>& doors, BufferObject& bo);
  void initWinningDoorsCubes(const int i, std::vector<Door>& doors, bx::Vec3& col, BufferObject& bo);

  void draw(const bool in_editor);
  void destroy();
};

#endif
