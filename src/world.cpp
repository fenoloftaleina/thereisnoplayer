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
  through_door.reserve(100);

  all_moving_spots.reserve(1000);
  all_any_through_doors.reserve(1000);

  moving_positions.reserve(100);
  moving_colors.reserve(100);
  static_positions.reserve(1000);
  static_colors.reserve(1000);
  doors_positions.reserve(1000);
  doors_colors.reserve(1000);
  winning_doors_positions.reserve(100);
  winning_doors_colors.reserve(100);
  floor_positions.reserve(10000);
  floor_colors.reserve(10000);
  editor_position.reserve(1);
  editor_color.reserve(1);

  positions_temp.reserve(100);
  colors_temp.reserve(100);
  models_temp.reserve(100);

  AnimatedPosColorTexVertex::init();

  moving_bo.initModels(100);
  static_bo.initModels(100);
  doors_bo.initCubes(1000);
  winning_doors_bo.initCubes(1000);
  floor_bo.initQuads(10000);
  editor_bo.initCubes(1);
  quads_bo.initQuads(1000);

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_animated_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_animated_simple.bin", "bin/f_simple.bin");
  doors_bo.createBuffers();
  doors_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  winning_doors_bo.createBuffers();
  winning_doors_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  floor_bo.createBuffers();
  floor_bo.createShaders("bin/v_tex.bin", "bin/f_tex.bin");
  editor_bo.createBuffers();
  editor_bo.createShaders("bin/v_simple.bin", "bin/f_editor.bin");
  quads_bo.createBuffers();
  quads_bo.createShaders("bin/v_tex.bin", "bin/f_tex.bin");

  std::vector<std::string> texture_assets = {
    "assets/t1.png",
    "assets/t2.png"
  };
  floor_bo.textures.prepare(texture_assets);
  quads_bo.textures.prepare(texture_assets);

  models.init();
  static_models_list.resize(1000);
  moving_models_list.resize(100);

  moving_nimate.prepare(this, &moving_bo, &moving_positions, &moving_colors, &moving_models_list);

  models_bo.initModels(100);
  models_bo.createBuffers();
  models_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
}


void World::init()
{
  won = false;
  moving_nimate.reset();

  moving_positions.resize(moving_spots.size());
  static_positions.resize(static_spots.size());
  doors_positions.resize(doors_spots.size());
  winning_doors_positions.resize(winning_doors_spots.size());
  floor_positions.resize(floor_spots.size());
  editor_position.resize(1);
  moving_colors.resize(moving_spots.size());
  static_colors.resize(static_spots.size());
  doors_colors.resize(doors_spots.size());
  winning_doors_colors.resize(winning_doors_spots.size());
  floor_colors.resize(floor_spots.size());
  editor_color.resize(1);
  through_door.resize(moving_spots.size());
  moving_cur_spots.resize(moving_spots.size());

  setPositionsFromSpots(moving_positions, moving_spots);
  setPositionsFromSpots(static_positions, static_spots);
  setPositionsFromSpots(doors_positions, doors_spots);
  setPositionsFromSpots(winning_doors_positions, winning_doors_spots);
  setPositionsFromSpots(floor_positions, floor_spots);
  setPositionsFromSpots(editor_position, editor_spot);

  setColors(moving_colors, moving_color);
  setColors(static_colors, static_color);
  for (int i = 0; i < doors_colors.size(); ++i) {
    doors_colors[i] = gate_colors[(int)(i / 2)];
  }
  setColors(winning_doors_colors, winning_doors_color);
  setColors(floor_colors, floor_color);
  setColors(editor_color, editor_thing_color);


  static_models_list.resize(static_positions.size());
  fr(i, static_positions) {
    static_models_list[i] = 3;
  }
  moving_models_list.resize(moving_positions.size());
  models_temp.resize(moving_models_list.size());
  fr(i, moving_positions) {
    moving_models_list[i] = 0;
    models_temp[i] = moving_models_list[i];
  }


  writeModelsVertices(moving_bo, moving_positions, moving_colors, moving_models_list);
  writeModelsVertices(static_bo, static_positions, static_colors, static_models_list);
  writeCubesVertices(doors_bo, doors_positions, doors_colors);
  writeCubesVertices(winning_doors_bo, winning_doors_positions, winning_doors_colors);
  writeFloorVertices(floor_bo, floor_positions, floor_colors, floor_mapping_ids);
  writeCubesVertices(editor_bo, editor_position, editor_color);

  moving_next_spots = moving_spots;


  moving_nimate.init();



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


  std::vector<bx::Vec3> quads_vs;
  std::vector<bx::Vec3> quads_cs;
  std::vector<int> quads_ms;
  const int grid_size = 50;
  const float tile_size = 2.0f;
  const float line_weight = 0.07f;
  quads_count = grid_size * 2 + 1;
  quads_vs.resize(quads_count * 4);
  quads_cs.resize(quads_count * 4);
  quads_ms.resize(quads_count);

  const float sx1 = -50.0f;
  const float sx2 =  50.0f;
  const float sy1 = -50.0f;
  const float sy2 =  50.0f;

  for (int i = 0; i < grid_size; ++i) {
    quads_vs[i * 4 + 0] = bx::Vec3(sx1 + i * tile_size + line_weight, -1.0f, sy1);
    quads_vs[i * 4 + 1] = bx::Vec3(sx1 + i * tile_size + line_weight, -1.0f, sy2);
    quads_vs[i * 4 + 2] = bx::Vec3(sx1 + i * tile_size - line_weight, -1.0f, sy1);
    quads_vs[i * 4 + 3] = bx::Vec3(sx1 + i * tile_size - line_weight, -1.0f, sy2);
  }

  for (int i = 0; i < grid_size; ++i) {
    quads_vs[(i + grid_size) * 4 + 0] = bx::Vec3(sx2, -1.0f, sy1 + i * tile_size - line_weight);
    quads_vs[(i + grid_size) * 4 + 1] = bx::Vec3(sx2, -1.0f, sy1 + i * tile_size + line_weight);
    quads_vs[(i + grid_size) * 4 + 2] = bx::Vec3(sx1, -1.0f, sy1 + i * tile_size - line_weight);
    quads_vs[(i + grid_size) * 4 + 3] = bx::Vec3(sx1, -1.0f, sy1 + i * tile_size + line_weight);
  }

  const bx::Vec3 grid_color(0.65f, 0.65f, 0.65f);
  for (int i = 0; i < quads_count - 1; ++i) {
    quads_cs[i * 4 + 0] = grid_color;
    quads_cs[i * 4 + 1] = grid_color;
    quads_cs[i * 4 + 2] = grid_color;
    quads_cs[i * 4 + 3] = grid_color;
  }

  for (int i = 0; i < quads_count - 1; ++i) {
    quads_ms[i] = -1;
  }

  const float plane_size = 10.0f;
  const bx::Vec3 plane_color(0.0f, 0.0f, 0.0f);
  const float plane_y = -1.01;
  quads_vs[(quads_count - 1) * 4 + 0] = bx::Vec3(plane_size, plane_y, -plane_size);
  quads_vs[(quads_count - 1) * 4 + 1] = bx::Vec3(plane_size, plane_y, plane_size);
  quads_vs[(quads_count - 1) * 4 + 2] = bx::Vec3(-plane_size, plane_y, -plane_size);
  quads_vs[(quads_count - 1) * 4 + 3] = bx::Vec3(-plane_size, plane_y, plane_size);
  quads_cs[(quads_count - 1) * 4 + 0] = plane_color;
  quads_cs[(quads_count - 1) * 4 + 1] = plane_color;
  quads_cs[(quads_count - 1) * 4 + 2] = plane_color;
  quads_cs[(quads_count - 1) * 4 + 3] = plane_color;
  quads_ms[quads_count - 1] = 0;
  quads_bo.writeQuadsVertices(0, quads_vs, quads_cs, quads_ms);
}


void World::updateBuffers()
{
  moving_bo.updateBuffer();
  static_bo.updateBuffer();
  doors_bo.updateBuffer();
  winning_doors_bo.updateBuffer();
  floor_bo.updateBuffer();

  quads_bo.updateBuffer();

  models_bo.updateBuffer();
}

void World::resolve
(const Spot& move, const bool in_editor, const bool back, const bool reset)
{
  made_move = false;
  travel = false;
  any_through_door = false;

  if (won || maybe_won()) {
    return;
  }

  if (in_editor) {
    make_editor_move(move);
    return;
  }

  if (reset) {
    execute_reset();
    return;
  }

  if (back) {
    execute_back();
    return;
  }

  maybe_make_move(move);
}


void World::update(const float t, const float dt)
{
  if (made_move) {
    positions_temp.resize(moving_spots.size());
    colors_temp.resize(moving_colors.size());
    models_temp.resize(moving_models_list.size());

    fr(i, colors_temp) {
      colors_temp[i] = bx::Vec3(0.5f, 0.7f, 0.9f);
    }

    fr(i, models_temp) {
      models_temp[i] = (models_temp[i] + 1) % 2;
    }

    setPositionsFromSpots(positions_temp, moving_cur_spots);

    animation_length = 150.0f;
    if (travel) { animation_length = 0.0f; }
    fr(i, positions_temp) {
      moving_nimate.schedule_position(i, positions_temp[i], t, t + animation_length);
      // moving_nimate.schedule_color(i, colors_temp[i], t, t + 1000.0f);
      moving_nimate.schedule_model(i, models_temp[i], t, t + animation_length * 2.0f);
    }

    acc_animation_length = t + animation_length;

    if (any_through_door) {
      setPositionsFromSpots(positions_temp, moving_spots);

      animation_length = 0.0f;
      fr(i, positions_temp) {
        if (through_door[i]) {
          moving_nimate.schedule_position(i, positions_temp[i],
              acc_animation_length, acc_animation_length + animation_length);
        }
      }
    }
  }

  moving_nimate.run(t);
}


void World::draw(const bool in_editor)
{
  moving_bo.drawModels(view, moving_spots.size(), models, 0, BGFX_STATE_BLEND_ALPHA);
  static_bo.drawModels(view, 0);
  doors_bo.drawCubes(view, doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  winning_doors_bo.drawCubes(view, winning_doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  floor_bo.drawQuads(view, floor_spots.size());

  quads_bo.drawQuads(view, quads_count);

  if (in_editor) {
    editor_bo.drawCubes(view, 1, BGFX_STATE_BLEND_ALPHA);
  }


  // models_bo.drawModels(view, 0);
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
  all_any_through_doors.push_back(any_through_door);
  moving_spots = moving_next_spots;
}


void World::maybe_doors()
{
  fr(i, moving_next_spots) {
    through_door[i] = false;
    moving_cur_spots[i] = moving_next_spots[i];
    fr(j, doors_spots) {
      if (same(moving_next_spots[i], doors_spots[j])) {
        towards = (((j + 1) % 2) * 2) - 1;
        moving_next_spots[i] = doors_spots[j + towards];
        j += (j + 1) % 2;
        through_door[i] = true;
        any_through_door = true;
      }
    }
  }
}


void World::execute_back()
{
  made_move = true;
  moving_spots = all_moving_spots.back();
  if (all_any_through_doors.back()) {
    travel = true;
  }
  moving_cur_spots = moving_spots;
  all_moving_spots.pop_back();
  all_any_through_doors.pop_back();
}


void World::execute_reset()
{
  made_move = true;
  travel = true;
  moving_nimate.reset();
  moving_spots = all_moving_spots.front();
  moving_cur_spots = moving_spots;
  all_moving_spots.clear();
  all_any_through_doors.clear();
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


void World::writeModelsVertices
(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors, const std::vector<int>& models_list)
{
  int acc_vertices_offset = 0;
  int acc_indices_offset = 0;

  for (int i = 0; i < positions.size(); ++i) {
    bo.writeModelVertices(
        acc_vertices_offset,
        positions[i],
        colors[i],
        models,
        models_list[i]
        );
    bo.writeModelIndices(
        acc_indices_offset,
        acc_vertices_offset,
        models,
        models_list[i]
        );

    acc_vertices_offset += models.nth_model_vertices_count(models_list[i]);
    acc_indices_offset += models.nth_model_indices_count(models_list[i]);
  }
}


void World::writeAnimatedModelsVertices
(BufferObject& bo,
 const std::vector<bx::Vec3>& positions1,
 const std::vector<bx::Vec3>& positions2,
 const std::vector<bx::Vec3>& colors1,
 const std::vector<bx::Vec3>& colors2,
 const std::vector<int> nth1s,
 const std::vector<int> nth2s,
 const std::vector<bx::Vec3>& froms,
 const std::vector<bx::Vec3>& tos
 )
{
  int acc_vertices_offset = 0;
  int acc_indices_offset = 0;

  for (int i = 0; i < positions1.size(); ++i) {
    bo.writeModelVertices(
        acc_vertices_offset,
        positions1[i],
        positions2[i],
        colors1[i],
        colors2[i],
        models,
        nth1s[i],
        nth2s[i],
        froms[i],
        tos[i]
        );
    bo.writeModelIndices(
        acc_indices_offset,
        acc_vertices_offset,
        models,
        nth1s[i]
        );

    acc_vertices_offset += models.nth_model_vertices_count(nth1s[i]);
    acc_indices_offset += models.nth_model_indices_count(nth2s[i]);
  }
}


void World::writeFloorVertices
(BufferObject& bo,
 const std::vector<bx::Vec3>& positions,
 const std::vector<bx::Vec3>& colors,
 const std::vector<int>& mapping_ids
 )
{
  std::vector<bx::Vec3> floor_vs;
  std::vector<bx::Vec3> floor_cs;
  int tile_size = 1.0f;
  floor_vs.resize(mapping_ids.size() * 4);
  floor_cs.resize(mapping_ids.size() * 4);

  for (int i = 0; i < mapping_ids.size(); ++i) {
    printf("f pos: %f %f %f, ", positions[i].x, positions[i].y, positions[i].z);
    floor_vs[i * 4 + 0] = bx::Vec3(positions[i].x + tile_size, -1.0f, positions[i].z - tile_size);
    floor_vs[i * 4 + 1] = bx::Vec3(positions[i].x + tile_size, -1.0f, positions[i].z + tile_size);
    floor_vs[i * 4 + 2] = bx::Vec3(positions[i].x - tile_size, -1.0f, positions[i].z - tile_size);
    floor_vs[i * 4 + 3] = bx::Vec3(positions[i].x - tile_size, -1.0f, positions[i].z + tile_size);
    floor_cs[i * 4 + 0] =
      floor_cs[i * 4 + 1] =
      floor_cs[i * 4 + 2] =
      floor_cs[i * 4 + 3] = colors[i];
  }

  bo.writeQuadsVertices(0, floor_vs, floor_cs, mapping_ids);
}
