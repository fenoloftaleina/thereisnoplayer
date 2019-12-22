#include "objs.hpp"
#include <cmath>

void Objs::initCube(const int i, Cube& cube, BufferObject& bo)
{
  cube.pos = Common::posOnSpot(cube.cur_spot);

  fraction = pow(2 * cube.anim_fraction - 1, 2);
  bo.writeCubeVertices(
      i,
      cube.pos,
      bx::add(
        bx::mul(cube.col, fraction),
        bx::mul(cube.on_door_col, (1.0f - fraction))
      ));
}

void Objs::initCube(const int i, Cube& cube, bx::Vec3& col, BufferObject& bo)
{
  cube.col = col;
  initCube(i, cube, bo);
}

void Objs::initCubes
(const int i, std::vector<Cube>& cubes, BufferObject& bo)
{
  for (int j = 0; j < cubes.size(); ++j) {
    initCube(i + j, cubes[j], cubes[j].col, bo);
  }

  bo.updateBuffer();
}

void Objs::initCubes
(const int i, std::vector<Cube>& cubes, bx::Vec3& col, BufferObject& bo)
{
  for (int j = 0; j < cubes.size(); ++j) {
    cubes[j].col = col;
  }

  initCubes(i, cubes, bo);
}

void Objs::initDoorsCubes(const int i, std::vector<Door>& doors, BufferObject& bo)
{
  for (int j = 0; j < doors.size(); ++j) {
    initCube(i + j, doors[j].cube, gate_colors[j / 2], bo);
  }

  bo.updateBuffer();
}

void Objs::initWinningDoorsCubes
(const int i, std::vector<Door>& doors, bx::Vec3& col, BufferObject& bo)
{
  for (int j = 0; j < doors.size(); ++j) {
    initCube(i + j, doors[j].cube, col, bo);
  }

  bo.updateBuffer();
}

void Objs::preInit()
{
  editor_cube.spot = {0, 5, 0};
  editor_cube.cur_spot = editor_cube.spot;
  previous_moving_cubes.reserve(10000);
  PosColorVertex::init();
  initBos();
}

void Objs::init()
{
  initCube(0, editor_cube, editor_cube_color, editor_bo);
  editor_bo.updateBuffer();
  initCubes(0, level->moving_cubes, moving_cubes_color, moving_bo);
  initCubes(0, level->static_cubes, static_cubes_color, static_bo);
  initDoorsCubes(0, level->doors, doors_bo);
  initWinningDoorsCubes(0, level->winning_doors, winning_doors_color, winning_doors_bo);
}

void Objs::initBos()
{
  moving_bo.initCubes(cubes_in_memory_count);
  static_bo.initCubes(cubes_in_memory_count);
  doors_bo.initCubes(doors_in_memory_count);
  winning_doors_bo.initCubes(doors_in_memory_count);
  editor_bo.initCubes(1);

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  doors_bo.createBuffers();
  doors_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  winning_doors_bo.createBuffers();
  winning_doors_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  editor_bo.createBuffers();
  editor_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
}

void Objs::draw(const bool in_editor)
{
  moving_bo.drawCubes(level->moving_cubes.size(), BGFX_STATE_BLEND_ALPHA);
  static_bo.drawCubes(level->static_cubes.size());
  doors_bo.drawCubes(level->doors.size(), BGFX_STATE_BLEND_ALPHA);
  winning_doors_bo.drawCubes(level->winning_doors.size(), BGFX_STATE_BLEND_ALPHA);

  if (in_editor) {
    editor_bo.drawCubes(1);
  }
}

void Objs::destroy()
{
  moving_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();
}


void World::prepare()
{
  moving_spots.reserve(100);
  moving_next_spots.reserve(100);
  moving_cur_spots.reserve(100);
  static_spots.reserve(1000);
  doors_spots.reserve(1000);
  winning_doors_spots.reserve(100);

  moving_positions.reserve(100);
  moving_colors.reserve(100);
  static_positions.reserve(1000);
  static_colors.reserve(1000);
  doors_positions.reserve(1000);
  doors_colors.reserve(1000);
  winning_doors_positions.reserve(100);
  winning_doors_colors.reserve(100);

  PosColorVertex::init();

  moving_bo.initCubes(1000);
  static_bo.initCubes(1000);
  doors_bo.initCubes(1000);
  winning_doors_bo.initCubes(1000);
  editor_bo.initCubes(1);

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  doors_bo.createBuffers();
  doors_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  winning_doors_bo.createBuffers();
  winning_doors_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  editor_bo.createBuffers();
  editor_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
}


void World::init()
{
  moving_positions.resize(moving_spots.size());
  static_positions.resize(static_spots.size());
  doors_positions.resize(doors_spots.size());
  winning_doors_positions.resize(winning_doors_spots.size());
  moving_colors.resize(moving_spots.size());
  static_colors.resize(static_spots.size());
  doors_colors.resize(doors_spots.size());
  winning_doors_colors.resize(winning_doors_spots.size());

  setPositionsFromSpots(moving_positions, moving_spots);
  setPositionsFromSpots(static_positions, static_spots);
  setPositionsFromSpots(doors_positions, doors_spots);
  setPositionsFromSpots(winning_doors_positions, winning_doors_spots);

  setColors(moving_colors, moving_color);
  setColors(static_colors, static_color);
  setColors(doors_colors, doors_color);
  setColors(winning_doors_colors, winning_doors_color);

  writeCubesVertices(moving_bo, moving_positions, moving_colors);
  writeCubesVertices(static_bo, static_positions, static_colors);
  writeCubesVertices(doors_bo, doors_positions, doors_colors);
  writeCubesVertices(winning_doors_bo, winning_doors_positions, winning_doors_colors);
}


void World::draw(const bool in_editor)
{
  moving_bo.drawCubes(moving_spots.size(), BGFX_STATE_BLEND_ALPHA);
  static_bo.drawCubes(static_spots.size());
  doors_bo.drawCubes(doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  winning_doors_bo.drawCubes(winning_doors_spots.size(), BGFX_STATE_BLEND_ALPHA);

  if (in_editor) {
    editor_bo.drawCubes(1);
  }
}


void World::destroy()
{
  moving_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();
}


void World::setPositionsFromSpots
(std::vector<bx::Vec3>& positions, const std::vector<Spot>& spots)
{
  for (int i = 0; i < spots.size(); ++i) {
    positions[i].x = (spots[i].x * 2.0f) - 5.0f;
    positions[i].y = 0.0f;
    positions[i].z = (spots[i].y * 2.0f) - 5.0f;
  }
}


void World::setColors(std::vector<bx::Vec3>& colors, const bx::Vec3& color)
{
  for (int i = 0; i < colors.size(); ++i) {
    colors[i] = color;
  }
}


void World::writeCubesVertices
(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors)
{
  for (int i = 0; i < positions.size(); ++i) {
    bo.writeCubeVertices(i, positions[i], colors[i]);
  }
}
