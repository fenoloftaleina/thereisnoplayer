#include "nimate.hpp"


void Nimate::prepare(
    World* _world,
    BufferObject* _bo,
    std::vector<bx::Vec3>* _positions,
    std::vector<bx::Vec3>* _colors
    )
{
  world = _world;
  bo = _bo;
  positions = _positions;
  colors = _colors;

  next_positions.reserve(10000);
  next_colors.reserve(10000);
  positions_from.reserve(10000);
  positions_to.reserve(10000);
  colors_from.reserve(10000);
  colors_to.reserve(10000);
  ids_positions.reserve(10000);
  ids_colors.reserve(10000);
  updated_positions.reserve(10000);
  updated_colors.reserve(10000);
  positions_temp.reserve(10000);
  colors_temp.reserve(10000);
  froms_temp.reserve(10000);
  tos_temp.reserve(10000);
}


void Nimate::schedule_position(
    const int id,
    const bx::Vec3& position,
    const float from,
    const float to
    )
{
  ids_positions.push_back(id);
  next_positions.push_back(position);
  positions_from.push_back(from);
  positions_to.push_back(to);
  updated_positions.push_back(false);
}


void Nimate::run(const float t, const bool update_anyway)
{
  needs_update = false;

  if (next_positions.empty() && next_colors.empty()) {
    return;
  }

  positions_temp.resize(positions->size());
  colors_temp.resize(colors->size());
  froms_temp.resize(positions->size());
  tos_temp.resize(positions->size());

  fr(i, positions_temp) {
    positions_temp[i] = (*positions)[i];
    colors_temp[i] = (*colors)[i];
  }

  fr(i, next_positions) {
    if (!updated_positions[i] &&
        positions_from[i] >= 0.0f &&
        t >= positions_from[i] &&
        (t <= positions_to[i] ||
         positions_to[i] - positions_from[i] < 0.001f)) {
      positions_temp[ids_positions[i]] = next_positions[i];
      froms_temp[ids_positions[i]].z = positions_from[i];
      tos_temp[ids_positions[i]].z = positions_to[i];
      updated_positions[i] = true;
      needs_update = true;
    }
  }

  fr(i, next_colors) {
    // colors_temp[ids_colors[i]] = next_colors[i];
    // froms_temp[i].y = colors_from[i];
    // tos_temp[i].y = colors_to[i];
  }

  fr(i, (*colors)) {
    colors_temp[i] = (*colors)[i];
  }

  if (needs_update) {
    // printf("UPDATE!\n");
    //
    // fr(i, positions_temp) {
    //   printf("%f %f %f - %f %f %f\n", (*positions)[i].x, (*positions)[i].y, (*positions)[i].z, positions_temp[i].x, positions_temp[i].y, positions_temp[i].z);
    // }
    // printf("\n");
    // fr(i, froms_temp) {
    //   printf("%f - %f (%f)\n", froms_temp[i].z, tos_temp[i].z, t);
    // }
    // printf("\n");

    world->writeAnimatedModelsVertices(
        *bo,
        *positions,
        positions_temp,
        *colors,
        colors_temp,
        0,
        0,
        froms_temp,
        tos_temp
        );
    bo->updateBuffer();

        fr(i, positions_temp) {
          (*positions)[i] = positions_temp[i];
        }

    fr(i, next_positions) {
      if (t > positions_to[i]) {
        next_positions.erase(next_positions.begin() + i);
        positions_from.erase(positions_from.begin() + i);
        positions_to.erase(positions_to.begin() + i);
        ids_positions.erase(ids_positions.begin() + i);
        updated_positions.erase(updated_positions.begin() + i);
        i -= 1;
      }
    }
  }
}


// void Nimate::abort(
//     std::vector<std::vector<bx::Vec3>>& next_values,
//     std::vector<float>& times
//     )
// {
//   if (!next_values.empty()) {
//     fr(i, times) {
//       times[i] = 999999.0f;
//     }
//   }
// }


void Nimate::reset()
{
  next_positions.clear();
  next_colors.clear();
  positions_from.clear();
  positions_to.clear();
  colors_from.clear();
  colors_to.clear();
  ids_positions.clear();
  ids_colors.clear();
}
