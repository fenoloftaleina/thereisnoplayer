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
  for (int j = 0; j < objs.doors.size(); j += 2) {
    if (Common::sameSpot(cube.next_spot, objs.doors[j].cube.spot)) {
      cube.next_spot = objs.doors[j + objs.doors[j].towards].cube.spot;
    } else if (Common::sameSpot(cube.next_spot, objs.doors[j + 1].cube.spot)) {
      cube.next_spot = objs.doors[j + 1 + objs.doors[j + 1].towards].cube.spot;
    }
  }
}

const bool SAME = true;
const bool WITH_MOVING = true;

bool Logic::movement(const bx::Vec3& cur_pos)
{
  for (int i = 0; i < objs.moving_cubes.size(); ++i) {
    objs.moving_cubes[i].next_spot = bx::add(objs.moving_cubes[i].spot, cur_pos);

    maybeDoors(objs.moving_cubes[i]);
  }

  return collidesWith(objs.moving_cubes, objs.static_cubes);
}

bool Logic::tryKidMovement(Cube& cube)
{
  int j;
  for (int i = 0; i < 4; ++i) {
    j = cube.kids_moves_offset % 4;

    cube.next_spot = cube.spot;
    cube.next_spot = bx::add(cube.next_spot, kids_moves[j][i]);

    maybeDoors(cube);

    if (!(collidesWith(cube, objs.static_cubes) ||
          collidesWith(cube, objs.kids_cubes, SAME, WITH_MOVING) ||
          collidesWith(cube, objs.moving_cubes, !SAME, WITH_MOVING))) {
      cube.kids_moves_offset = j;

      return false;
    }
  }

  return true;
}

bool Logic::kidsMovement()
{
  for (int i = 0; i < objs.kids_cubes.size(); ++i) {
    if (tryKidMovement(objs.kids_cubes[i])) {
      return true;
    }
  }

  return false;
}

bool Logic::run(const bx::Vec3& cur_pos, const bool in_editor)
{
  winning_count = 0;
  for (int i = 0; i < objs.moving_cubes.size(); ++i) {
    for (int j = 0; j < objs.winning_doors.size(); ++j) {
      if (Common::sameSpot(objs.moving_cubes[i].spot, objs.winning_doors[j].cube.spot)) {
        winning_count += 1;
      }
    }
  }

  if (winning_count == objs.winning_doors.size() && objs.winning_doors.size() > 0) {
    return true;
  }

  if (cur_pos.x == 0.0f && cur_pos.y == 0.0f && cur_pos.z == 0.0f) {
    return false;
  }

  if (in_editor) {
    editor(cur_pos);
    return false;
  }

  were_collisions = false;

  were_collisions = movement(cur_pos);
    // || kidsMovement();

  if (were_collisions) {
    printf("COLLISIONS!\n");
    return false;
  }

  for (int i = 0; i < objs.moving_cubes.size(); ++i) {
    objs.moving_cubes[i].spot = objs.moving_cubes[i].next_spot;
  }

  for (int i = 0; i < objs.kids_cubes.size(); ++i) {
    objs.kids_cubes[i].spot = objs.kids_cubes[i].next_spot;
  }

  objs.initCubes(0, objs.moving_cubes, objs.moving_bo);
  objs.initCubes(0, objs.kids_cubes, objs.kids_bo);

  return false;
}

void Logic::editor(const bx::Vec3& cur_pos)
{
  objs.editor_cube.next_spot = bx::add(objs.editor_cube.spot, cur_pos);
  objs.editor_cube.spot = objs.editor_cube.next_spot;

  objs.initCube(0, objs.editor_cube, objs.editor_bo);
  objs.editor_bo.updateBuffer();
}
