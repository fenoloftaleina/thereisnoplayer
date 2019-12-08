#include "logic.hpp"

bool Logic::collidesWith(const Cube& cube, const std::vector<Cube>& cubes, const int start_j = 0, const bool with_moving = false)
{
  for (int j = start_j; j < cubes.size(); ++j) {
    if ((with_moving && Common::sameSpot(cube.next_spot, cubes[j].next_spot)) ||
        (!with_moving && Common::sameSpot(cube.next_spot, cubes[j].spot))) {
      return true;
    }
  }

  return false;
}

bool Logic::collidesWith(const std::vector<Cube>& cubes1, const std::vector<Cube>& cubes2, const bool same = false, const bool with_moving = false)
{
  int start_j;

  for (int i = 0; i < cubes1.size(); ++i) {
    if (same) {
      start_j = i + 1;
    } else {
      start_j = 0;
    }

    if (collidesWith(cubes1[i], cubes2, start_j, with_moving)) {
      return true;
    }
  }

  return false;
}

void Logic::maybeDoors(Cube& cube)
{
  cube.on_door = true;

  for (int j = 0; j < level->doors.size(); j += 2) {
    if (Common::sameSpot(cube.next_spot, level->doors[j].cube.spot)) {
      cube.next_spot = level->doors[j + level->doors[j].towards].cube.spot;
      cube.cur_spot = level->doors[j].cube.spot;
      cube.on_door_col = level->doors[j].cube.col;
      return;
    } else if (Common::sameSpot(cube.next_spot, level->doors[j + 1].cube.spot)) {
      cube.next_spot = level->doors[j + 1 + level->doors[j + 1].towards].cube.spot;
      cube.cur_spot = level->doors[j + 1].cube.spot;
      cube.on_door_col = level->doors[j + 1].cube.col;
      return;
    }
  }

  cube.on_door = false;
}

const bool SAME = true;
const bool WITH_MOVING = true;

bool Logic::movement(const bx::Vec3& cur_pos)
{
  for (int i = 0; i < level->moving_cubes.size(); ++i) {
    level->moving_cubes[i].next_spot = bx::add(level->moving_cubes[i].spot, cur_pos);

    maybeDoors(level->moving_cubes[i]);
  }

  return collidesWith(level->moving_cubes, level->static_cubes);
}

bool Logic::run(const bx::Vec3& cur_pos, const bool in_editor, const bool back, const float dt)
{
  winning_count = 0;
  for (int i = 0; i < level->moving_cubes.size(); ++i) {
    for (int j = 0; j < level->winning_doors.size(); ++j) {
      if (Common::sameSpot(level->moving_cubes[i].spot, level->winning_doors[j].cube.spot)) {
        winning_count += 1;
      }
    }
  }

  for (auto& cube : level->moving_cubes) {
    if (cube.anim_fraction > 0.5f) {
      cube.cur_spot = cube.spot;
    }

    // cube.anim_fraction += dt * 0.0025;
    cube.anim_fraction += dt * 0.004;
    if (cube.anim_fraction > 1.0f) cube.anim_fraction = 1.0f;
    objs->initCubes(0, level->moving_cubes, objs->moving_bo);
  }

  if (winning_count == level->winning_doors.size() && level->winning_doors.size() > 0) {
    return true;
  }

  if (cur_pos.x == 0.0f && cur_pos.y == 0.0f && cur_pos.z == 0.0f) {
    if (back) objs->initCubes(0, level->moving_cubes, objs->moving_bo);
    return false;
  }

  if (in_editor) {
    editor(cur_pos);
    return false;
  }

  if (!back) objs->previous_moving_cubes.push_back(level->moving_cubes);

  were_collisions = movement(cur_pos);

  if (were_collisions) {
    return false;
  }

  for (int i = 0; i < level->moving_cubes.size(); ++i) {
    if (level->moving_cubes[i].on_door) {
      level->moving_cubes[i].anim_fraction = 0.0f;
    } else {
      level->moving_cubes[i].cur_spot = level->moving_cubes[i].next_spot;
    }
    level->moving_cubes[i].spot = level->moving_cubes[i].next_spot;
  }

  objs->initCubes(0, level->moving_cubes, objs->moving_bo);

  return false;
}

void Logic::editor(const bx::Vec3& cur_pos)
{
  objs->editor_cube.next_spot = bx::add(objs->editor_cube.spot, cur_pos);
  objs->editor_cube.spot = objs->editor_cube.next_spot;

  objs->initCube(0, objs->editor_cube, objs->editor_bo);
  objs->editor_bo.updateBuffer();
}
