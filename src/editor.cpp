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
  removeOnIdx(objs->moving_cubes, idxForSpot(objs->moving_cubes, spot)) ||
    removeOnIdx(objs->kids_cubes, idxForSpot(objs->kids_cubes, spot)) ||
    removeOnIdx(objs->static_cubes, idxForSpot(objs->static_cubes, spot)) ||
    removeOnIdx(objs->doors, idxForSpot(objs->doors, spot)) ||
    removeOnIdx(objs->winning_doors, idxForSpot(objs->winning_doors, spot));
}

void Editor::addMovingCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = objs->moving_cubes_color;
  c.spot = spot;
  c.next_spot = spot;
  c.pos = Common::posOnSpot(c.spot);
  objs->moving_cubes.push_back(c);
}

void Editor::addKidsCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = objs->kids_cubes_color;
  c.spot = spot;
  c.next_spot = spot;
  c.pos = Common::posOnSpot(c.spot);
  objs->kids_cubes.push_back(c);
}

void Editor::addStaticCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = objs->static_cubes_color;
  c.spot = spot;
  c.next_spot = spot;
  c.pos = Common::posOnSpot(c.spot);
  objs->static_cubes.push_back(c);
}

void Editor::addOrUpdateDoor(const bx::Vec3& spot)
{
  Door d;
  d.cube.col = objs->doors_color;
  d.cube.spot = spot;
  d.cube.next_spot = spot;
  d.cube.pos = Common::posOnSpot(d.cube.spot);
  int idx = idxForSpot(objs->doors, spot);
  if (idx == -1) {
    if (!objs->doors.empty()) {
      d.towards = objs->doors.back().towards * -1;
    } else {
      d.towards = 1;
    }
    objs->doors.push_back(d);
  } else {
    d.towards = objs->doors[idx].towards;
    objs->doors[idx] = d;
  }
}


void Editor::addWinningDoor(const bx::Vec3& spot)
{
  Door d;
  d.cube.col = objs->winning_doors_color;
  d.cube.spot = spot;
  d.cube.next_spot = spot;
  d.cube.pos = Common::posOnSpot(d.cube.spot);
  objs->winning_doors.push_back(d);
}
