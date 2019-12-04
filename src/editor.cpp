#include "editor.hpp"
#include "common.hpp"

int Editor::idxForSpot(const std::vector<Cube>& v, const bx::Vec3& spot)
{
  for (int i = 0; i < v.size(); ++i) {
    if (Common::sameSpot(v[i].spot, spot)) {
      return i;
    }
  }

  return -1;
}

int Editor::idxForSpot(const std::vector<Door>& v, const bx::Vec3& spot)
{
  for (int i = 0; i < v.size(); ++i) {
    if (Common::sameSpot(v[i].cube.spot, spot)) {
      return i;
    }
  }

  return -1;
}

bool Editor::removeOnIdx(std::vector<Cube>& v, int i)
{
  if (i != -1) {
    v.erase(v.begin() + i);
    return true;
  }

  return false;
}

bool Editor::removeOnIdx(std::vector<Door>& v, int i)
{
  if (i != -1) {
    v.erase(v.begin() + i);
    return true;
  }

  return false;
}

void Editor::removeOnSpot(const bx::Vec3& spot)
{
  removeOnIdx(level->moving_cubes, idxForSpot(level->moving_cubes, spot)) ||
    removeOnIdx(level->static_cubes, idxForSpot(level->static_cubes, spot)) ||
    removeOnIdx(level->doors, idxForSpot(level->doors, spot)) ||
    removeOnIdx(level->winning_doors, idxForSpot(level->winning_doors, spot));
}

void Editor::addMovingCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = objs->moving_cubes_color;
  c.spot = spot;
  c.next_spot = spot;
  c.pos = Common::posOnSpot(c.spot);
  level->moving_cubes.push_back(c);
}

void Editor::addStaticCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = objs->static_cubes_color;
  c.spot = spot;
  c.next_spot = spot;
  c.pos = Common::posOnSpot(c.spot);
  level->static_cubes.push_back(c);
}

void Editor::addOrUpdateDoor(const bx::Vec3& spot)
{
  Door d;
  d.cube.col = objs->doors_color;
  d.cube.spot = spot;
  d.cube.next_spot = spot;
  d.cube.pos = Common::posOnSpot(d.cube.spot);
  int idx = idxForSpot(level->doors, spot);
  if (idx == -1) {
    if (!level->doors.empty()) {
      d.towards = level->doors.back().towards * -1;
    } else {
      d.towards = 1;
    }
    level->doors.push_back(d);
  } else {
    d.towards = level->doors[idx].towards;
    level->doors[idx] = d;
  }
}


void Editor::addWinningDoor(const bx::Vec3& spot)
{
  Door d;
  d.cube.col = objs->winning_doors_color;
  d.cube.spot = spot;
  d.cube.next_spot = spot;
  d.cube.pos = Common::posOnSpot(d.cube.spot);
  level->winning_doors.push_back(d);
}
