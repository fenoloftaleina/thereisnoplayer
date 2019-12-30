#include "world.hpp"
#include <cmath>

void World::prepare()
{
  moving_spots.reserve(100);
  moving_next_spots.reserve(100);
  moving_cur_spots.reserve(100);
  static_spots.reserve(1000);
  doors_spots.reserve(1000);
  winning_doors_spots.reserve(100);
  editor_spot.reserve(1);
  editor_spot.resize(1);

  all_moving_spots.reserve(1000);

  moving_positions.reserve(100);
  moving_colors.reserve(100);
  static_positions.reserve(1000);
  static_colors.reserve(1000);
  doors_positions.reserve(1000);
  doors_colors.reserve(1000);
  winning_doors_positions.reserve(100);
  winning_doors_colors.reserve(100);
  editor_position.reserve(1);
  editor_color.reserve(1);

  positions_temp.reserve(100);
  colors_temp.reserve(100);
  lengths_temp.reserve(100);

  PosColorVertex::init();

  moving_bo.initModels(100);
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

  models.init();

  models_bo.initModels(100);
  models_bo.createBuffers();
  models_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
}


void World::init()
{
  moving_positions.resize(moving_spots.size());
  static_positions.resize(static_spots.size());
  doors_positions.resize(doors_spots.size());
  winning_doors_positions.resize(winning_doors_spots.size());
  editor_position.resize(1);
  moving_colors.resize(moving_spots.size());
  static_colors.resize(static_spots.size());
  doors_colors.resize(doors_spots.size());
  winning_doors_colors.resize(winning_doors_spots.size());
  editor_color.resize(1);

  setPositionsFromSpots(moving_positions, moving_spots);
  setPositionsFromSpots(static_positions, static_spots);
  setPositionsFromSpots(doors_positions, doors_spots);
  setPositionsFromSpots(winning_doors_positions, winning_doors_spots);
  setPositionsFromSpots(editor_position, editor_spot);

  setColors(moving_colors, moving_color);
  setColors(static_colors, static_color);
  for (int i = 0; i < doors_colors.size(); ++i) {
    doors_colors[i] = gate_colors[(int)(i / 2)];
  }
  setColors(winning_doors_colors, winning_doors_color);
  setColors(editor_color, editor_thing_color);

  writeModelsVertices(moving_bo, moving_positions, moving_colors, 0);
  writeCubesVertices(static_bo, static_positions, static_colors);
  writeCubesVertices(doors_bo, doors_positions, doors_colors);
  writeCubesVertices(winning_doors_bo, winning_doors_positions, winning_doors_colors);
  writeCubesVertices(editor_bo, editor_position, editor_color);

  moving_next_spots = moving_spots;


  models_bo.writeModelVertices(
      0,
      bx::Vec3(-2.0f, 3.0f, 0.0f),
      bx::Vec3(0.5f, 0.3f, 0.9f),
      models,
      0
      );
  models_bo.writeModelIndices(0, 0, models, 0);
  models_bo.writeModelVertices(
      models.nth_model_vertices_count(0),
      bx::Vec3(2.0f, 0.0f, 0.0f),
      bx::Vec3(0.1f, 0.3f, 0.9f),
      models,
      0
      );
  models_bo.writeModelIndices(
      models.nth_model_indices_count(0),
      models.nth_model_vertices_count(0),
      models,
      0
      );
}


void World::updateBuffers()
{
  moving_bo.updateBuffer();
  static_bo.updateBuffer();
  doors_bo.updateBuffer();
  winning_doors_bo.updateBuffer();

  models_bo.updateBuffer();
}


void World::resolve(const Spot& move, const bool in_editor, const bool back)
{
  made_move = false;

  if (maybe_won()) {
    return;
  }

  if (in_editor) {
    make_editor_move(move);
    return;
  }

  if (back) {
    execute_back();
    return;
  }

  maybe_make_move(move);
  maybe_doors();
}


void World::update(const float dt)
{
  if (made_move) {
    positions_temp.resize(moving_spots.size());
    setPositionsFromSpots(positions_temp, moving_spots);
    lengths_temp.resize(moving_spots.size());
    fr(i, lengths_temp) { lengths_temp[i] = 100.0f; }

    // nimate.abort(
    //     nimate.next_moving_positions,
    //     nimate.moving_positions_times
    //     );

    // fr(i, nimate.moving_positions_times) {
    //   nimate.moving_positions_times[i] += 50.0f;
    // }

    nimate.schedule(
        nimate.next_moving_positions,
        positions_temp,
        nimate.next_moving_positions_lengths,
        lengths_temp,
        nimate.moving_positions_times
        );
  } else if (!nimate.next_moving_positions.empty()) {
    nimate.run(
        dt,
        moving_positions,
        nimate.next_moving_positions,
        nimate.moving_positions_times,
        nimate.next_moving_positions_lengths
        );
  }

  writeModelsVertices(moving_bo, moving_positions, moving_colors, 0);
  moving_bo.updateBuffer();
}


void World::reset()
{
  if (!all_moving_spots.empty()) {
    moving_spots = all_moving_spots.front();
    all_moving_spots.clear();
  }
}


void World::draw(const bool in_editor)
{
  moving_bo.drawModels(moving_spots.size(), models, 0, BGFX_STATE_BLEND_ALPHA);
  static_bo.drawCubes(static_spots.size());
  doors_bo.drawCubes(doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  winning_doors_bo.drawCubes(winning_doors_spots.size(), BGFX_STATE_BLEND_ALPHA);

  if (in_editor) {
    editor_bo.drawCubes(1);
  }


  // models_bo.drawModels(0);
}


void World::destroy()
{
  moving_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();


  models_bo.destroy();
}


bool World::same(const Spot& s1, const Spot& s2) {return s1.x == s2.x && s1.y == s2.y;}
void World::equals_sum(Spot& e, const Spot& s1, const Spot& s2) { e.x = s1.x + s2.x; e.y = s1.y + s2.y; }

bool World::maybe_won()
{
  winning_count = 0;

  fr(i, moving_spots) {
    fr(j, winning_doors_spots) {
      winning_count += same(moving_spots[i], winning_doors_spots[j]);
    }
  }

  won = winning_count == winning_doors_spots.size();

  return won;
}


void World::maybe_make_move(const Spot& move)
{
  if (move.x == 0 && move.y == 0) {
    return;
  }

  fr(i, moving_next_spots) {
    equals_sum(moving_next_spots[i], moving_spots[i], move);
  }

  fr(i, moving_next_spots) {
    fr(j, static_spots) {
      if (same(moving_next_spots[i], static_spots[j])) {
        return;
      }
    }
  }

  made_move = true;

  maybe_doors();

  all_moving_spots.push_back(moving_spots);
  moving_spots = moving_next_spots;
}


void World::maybe_doors()
{
  fr(i, moving_next_spots) {
    fr(j, doors_spots) {
      if (same(moving_next_spots[i], doors_spots[j])) {
        towards = (((j + 1) % 2) * 2) - 1;
        moving_next_spots[i] = doors_spots[j + towards];
        j += (j + 1) % 2;
      }
    }
  }
}


void World::execute_back()
{
  moving_spots = all_moving_spots.back();
  all_moving_spots.pop_back();
}


void World::make_editor_move(const Spot& move)
{
  if (move.x == 0 && move.y == 0) {
    return;
  }

  equals_sum(editor_spot[0], editor_spot[0], move);
  setPositionsFromSpots(editor_position, editor_spot);
  writeCubesVertices(editor_bo, editor_position, editor_color);
  editor_bo.updateBuffer();
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


void World::writeModelsVertices
(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors, const int nth_model)
{
  int nth_model_vertices_count = models.nth_model_vertices_count(nth_model);
  int nth_model_indices_count = models.nth_model_indices_count(nth_model);

  for (int i = 0; i < positions.size(); ++i) {
    bo.writeModelVertices(
        nth_model_vertices_count * i,
        positions[i],
        colors[i],
        models,
        nth_model
        );
    bo.writeModelIndices(
        nth_model_indices_count * i,
        nth_model_vertices_count * i,
        models,
        nth_model
        );
  }
}
