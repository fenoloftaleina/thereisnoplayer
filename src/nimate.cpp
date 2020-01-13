#include "nimate.hpp"


void Nimate::prepare(
    World* _world,
    BufferObject* _bo,
    std::vector<bx::Vec3>* _positions,
    std::vector<bx::Vec3>* _colors,
    std::vector<int>* _models
    )
{
  world = _world;
  bo = _bo;
  positions = _positions;
  colors = _colors;
  models = _models;

  next_positions.reserve(10000);
  next_colors.reserve(10000);
  next_models.reserve(10000);
  positions_from.reserve(10000);
  positions_to.reserve(10000);
  colors_from.reserve(10000);
  colors_to.reserve(10000);
  models_from.reserve(10000);
  models_to.reserve(10000);
  ids_positions.reserve(10000);
  ids_colors.reserve(10000);
  ids_models.reserve(10000);
  updated_positions.reserve(10000);
  updated_colors.reserve(10000);
  updated_models.reserve(10000);
  positions_temp.reserve(10000);
  colors_temp.reserve(10000);
  models_temp.reserve(10000);
  froms_temp.reserve(10000);
  tos_temp.reserve(10000);
}


void Nimate::init()
{
  positions_temp.resize(positions->size());
  colors_temp.resize(colors->size());
  models_temp.resize(models->size());
  froms_temp.resize(positions->size());
  tos_temp.resize(positions->size());

  fr(i, (*positions)) {
    positions_temp[i] = (*positions)[i];
    colors_temp[i] = (*colors)[i];
    models_temp[i] = (*models)[i];
  }
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


void Nimate::schedule_color(
    const int id,
    const bx::Vec3& color,
    const float from,
    const float to
    )
{
  ids_colors.push_back(id);
  next_colors.push_back(color);
  colors_from.push_back(from);
  colors_to.push_back(to);
  updated_colors.push_back(false);
}


void Nimate::schedule_model(
    const int id,
    const int nth_model,
    const float from,
    const float to
    )
{
  ids_models.push_back(id);
  next_models.push_back(nth_model);
  models_from.push_back(from);
  models_to.push_back(to);
  updated_models.push_back(false);
}


void Nimate::run(const float t, const bool update_anyway)
{
  needs_update = false;

  if (next_positions.empty() && next_colors.empty()) {
    return;
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
    if (!updated_colors[i] &&
        colors_from[i] >= 0.0f &&
        t >= colors_from[i] &&
        (t <= colors_to[i] ||
         colors_to[i] - colors_from[i] < 0.001f)) {
      colors_temp[ids_colors[i]] = next_colors[i];
      froms_temp[ids_colors[i]].y = colors_from[i];
      tos_temp[ids_colors[i]].y = colors_to[i];
      updated_colors[i] = true;
      needs_update = true;
    }
  }

  fr(i, next_models) {
    if (!updated_models[i] &&
        models_from[i] >= 0.0f &&
        t >= models_from[i] &&
        (t <= models_to[i] ||
         models_to[i] - models_from[i] < 0.001f)) {
      models_temp[ids_models[i]] = next_models[i];
      froms_temp[ids_models[i]].x = models_from[i];
      tos_temp[ids_models[i]].x = models_to[i];
      updated_models[i] = true;
      needs_update = true;
    }
  }

  if (needs_update) {
    // printf("UPDATE!\n");
    //
    // fr(i, positions_temp) {
    //   printf("%f %f %f - %f %f %f\n", (*positions)[i].x, (*positions)[i].y, (*positions)[i].z, positions_temp[i].x, positions_temp[i].y, positions_temp[i].z);
    // }
    // printf("\n");
    // fr(i, models_temp) {
    //   printf("%d -> %d\n", (*models)[i], models_temp[i]);
    // }
    // printf("\n");
    // fr(i, froms_temp) {
    //   printf("%f - %f (%f)\n", froms_temp[i].x, tos_temp[i].x, t);
    // }
    // printf("\n");

    world->writeAnimatedModelsVertices(
        *bo,
        *positions,
        positions_temp,
        *colors,
        colors_temp,
        *models,
        models_temp,
        froms_temp,
        tos_temp
        );
    bo->updateBuffer();

    fr(i, positions_temp) {
      (*positions)[i] = positions_temp[i];
    }

    fr(i, colors_temp) {
      (*colors)[i] = colors_temp[i];
    }

    fr(i, models_temp) {
      (*models)[i] = models_temp[i];
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

    fr(i, next_colors) {
      if (t > colors_to[i]) {
        next_colors.erase(next_colors.begin() + i);
        colors_from.erase(colors_from.begin() + i);
        colors_to.erase(colors_to.begin() + i);
        ids_colors.erase(ids_colors.begin() + i);
        updated_colors.erase(updated_colors.begin() + i);
        i -= 1;
      }
    }

    fr(i, next_models) {
      if (t > models_to[i]) {
        next_models.erase(next_models.begin() + i);
        models_from.erase(models_from.begin() + i);
        models_to.erase(models_to.begin() + i);
        ids_models.erase(ids_models.begin() + i);
        updated_models.erase(updated_models.begin() + i);
        i -= 1;
      }
    }
  }
}


void Nimate::reset()
{
  next_positions.clear();
  next_colors.clear();
  next_models.clear();
  positions_from.clear();
  positions_to.clear();
  colors_from.clear();
  colors_to.clear();
  models_from.clear();
  models_to.clear();
  ids_positions.clear();
  ids_colors.clear();
  ids_models.clear();
  updated_positions.clear();
  updated_colors.clear();
  updated_models.clear();
}
