#ifndef OBJS
#define OBJS
#pragma once

#include "buffer_object.hpp"
#include "common.hpp"

struct Cube {
  bx::Vec3 pos {0.0f, 0.0f, 0.0f};
  bx::Vec3 col {0.7f, 0.7f, 0.7f};

  bx::Vec3 spot {0, 0, 0};
  bx::Vec3 next_spot {0, 0, 0};

  int kids_moves_offset = 0;

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

struct Objs
{
  BufferObject moving_bo;
  BufferObject kids_bo;
  BufferObject static_bo;
  BufferObject doors_bo;
  BufferObject winning_doors_bo;
  BufferObject editor_bo;

  std::vector<Cube> moving_cubes;
  std::vector<Cube> kids_cubes;
  std::vector<Cube> static_cubes;
  Cube editor_cube;

  std::vector<Door> doors;
  std::vector<Door> winning_doors;

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
