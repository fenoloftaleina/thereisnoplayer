#include "editor.hpp"
#include "common.hpp"

void Editor::add(std::vector<Spot>& spots, const Spot& spot, BufferObject& bo)
{
  spots.push_back(spot);
  world->init();
  bo.updateBuffer();
}


void Editor::remove(const Spot& spot)
{
  if (remove_for(world->moving_spots, spot) ||
      remove_for(world->static_spots, spot) ||
      remove_for(world->doors_spots, spot) ||
      remove_for(world->winning_doors_spots, spot)) {
    world->init();
    world->updateBuffers();
  }
}


int Editor::find(const std::vector<Spot>& spots, const Spot& spot)
{
  fr(i, spots) {
    if (World::same(spots[i], spot)) {
      return i;
    }
  }

  return -1;
}


bool Editor::remove_for(std::vector<Spot>& spots, const Spot& spot)
{
  id = find(spots, spot);
  if (id != -1) {
    spots.erase(spots.begin() + id);
    return true;
  }

  return false;
}
