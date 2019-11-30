#include "objs.hpp"

void Objs::initCube(const int i, Cube& cube, BufferObject& bo)
{
  cube.pos = Common::posOnSpot(cube.spot);
  bo.writeCubeVertices(i, cube.pos, cube.col);
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
  moving_cubes.reserve(cubes_in_memory_count);
  kids_cubes.reserve(cubes_in_memory_count);
  static_cubes.reserve(cubes_in_memory_count);
  doors.reserve(doors_in_memory_count);
  winning_doors.reserve(doors_in_memory_count);
  editor_cube.spot = {0, 5, 0};
  bg_cube.spot = {0, 4, 0};
  PosColorVertex::init();
  initBos();
}

void Objs::init()
{
  initCube(0, editor_cube, editor_cube_color, editor_bo);
  editor_bo.updateBuffer();

  bg_cube.pos = Common::posOnSpot(bg_cube.spot);
  bg_bo.writeCubeVertices(0, bg_cube.pos, editor_cube_color, bx::Vec3(100.0f, 1.0f, 100.0f));
  bg_bo.updateBuffer();

  initCubes(0, moving_cubes, moving_cubes_color, moving_bo);
  initCubes(0, kids_cubes, kids_cubes_color, kids_bo);
  initCubes(0, static_cubes, static_cubes_color, static_bo);
  initDoorsCubes(0, doors, doors_bo);
  initWinningDoorsCubes(0, winning_doors, winning_doors_color, winning_doors_bo);
}

void Objs::initBos()
{
  moving_bo.initCubes(cubes_in_memory_count);
  kids_bo.initCubes(cubes_in_memory_count);
  static_bo.initCubes(cubes_in_memory_count);
  doors_bo.initCubes(doors_in_memory_count);
  winning_doors_bo.initCubes(doors_in_memory_count);
  editor_bo.initCubes(1);
  bg_bo.initCubes(1);

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  kids_bo.createBuffers();
  kids_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  doors_bo.createBuffers();
  doors_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  winning_doors_bo.createBuffers();
  winning_doors_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  editor_bo.createBuffers();
  editor_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  bg_bo.createBuffers();
  bg_bo.createShaders("bin/v_simple.bin", "bin/f_bg.bin");
}

void Objs::draw(const bool in_editor)
{
  bg_bo.drawCubes(1);
  moving_bo.drawCubes(moving_cubes.size());
  kids_bo.drawCubes(kids_cubes.size());
  static_bo.drawCubes(static_cubes.size());
  doors_bo.drawCubes(doors.size());
  winning_doors_bo.drawCubes(winning_doors.size());

  if (in_editor) {
    editor_bo.drawCubes(1);
  }
}

void Objs::destroy()
{
  moving_bo.destroy();
  kids_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();
}
