#include "world.hpp"
#include <cmath>


void gen_quads_with_edges(
    const std::vector<bx::Vec3>& points,
    std::vector<bx::Vec3>& vertices,
    std::vector<bx::Vec3>& colors,
    std::vector<bx::Vec3>& normals,
    std::vector<bx::Vec3>& uvs,
    std::vector<int>& indices
    )
{
  float lw = 0.02f;
  bx::Vec3 a, b, c, normal;
  int planes = 5;
  int quads = planes * points.size() / 4;
  printf("PLANES %d!\n\n", planes);

  std::vector<bx::Vec3> inner;
  inner.resize(4);

  vertices.resize(quads * 4);
  colors.resize(quads * 4);
  normals.resize(quads * 4);
  uvs.resize(quads * 4);
  indices.resize(quads * 6);

  bx::Vec3 inside_color(0.9f, 0.9f, 0.9f);
  bx::Vec3 border_color(0.2f, 0.2f, 0.2f);

  fr(i, colors) {
    colors[i] = border_color;
    uvs[i] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  }

  for (int i = 0; i < points.size(); i += 4) {
    a = bx::Vec3(
        points[i + 1].x,
        points[i + 1].y,
        points[i + 1].z
        );
    b = bx::Vec3(
        points[i + 0].x,
        points[i + 0].y,
        points[i + 0].z
        );
    c = bx::Vec3(
        points[i + 3].x,
        points[i + 3].y,
        points[i + 3].z
        );

    normal = bx::normalize(
        bx::cross(
          bx::sub(a, b),
          bx::sub(a, c)
          )
        );

    for(int j = 0; j < 4 * planes; ++j) {
      normals[i * planes + j] = normal;
    }

    colors[i * planes + 0] =
      colors[i * planes + 1] =
      colors[i * planes + 2] =
      colors[i * planes + 3] =
      inside_color;

    inner[0] = bx::add(bx::mul(bx::sub(points[i + 3], points[i + 0]), lw), points[i + 0]);
    inner[1] = bx::add(bx::mul(bx::sub(points[i + 2], points[i + 1]), lw), points[i + 1]);
    inner[2] = bx::add(bx::mul(bx::sub(points[i + 1], points[i + 2]), lw), points[i + 2]);
    inner[3] = bx::add(bx::mul(bx::sub(points[i + 0], points[i + 3]), lw), points[i + 3]);


    // inside
    vertices[i * planes + 0] = inner[0];
    vertices[i * planes + 1] = inner[1];
    vertices[i * planes + 2] = inner[2];
    vertices[i * planes + 3] = inner[3];

    // Common::pv3(inner[0]);
    // Common::pv3(inner[1]);
    // Common::pv3(inner[2]);
    // Common::pv3(inner[3]);

    // left
    vertices[i * planes + 4] = inner[2];
    vertices[i * planes + 5] = inner[3];
    vertices[i * planes + 6] = points[i + 2];
    vertices[i * planes + 7] = points[i + 3];

    // right
    vertices[i * planes + 8] = points[i + 0];
    vertices[i * planes + 9] = points[i + 1];
    vertices[i * planes + 10] = inner[0];
    vertices[i * planes + 11] = inner[1];

    // bottom
    vertices[i * planes + 12] = points[i + 0];
    vertices[i * planes + 13] = inner[0];
    vertices[i * planes + 14] = points[i + 2];
    vertices[i * planes + 15] = inner[2];

    // top
    vertices[i * planes + 16] = inner[1];
    vertices[i * planes + 17] = points[i + 1];
    vertices[i * planes + 18] = inner[3];
    vertices[i * planes + 19] = points[i + 3];
  }

  for (int i = 0; i < quads; ++i) {
    indices[6 * i + 0] = 4 * i + 1;
    indices[6 * i + 1] = 4 * i + 0;
    indices[6 * i + 2] = 4 * i + 3;
    indices[6 * i + 3] = 4 * i + 0;
    indices[6 * i + 4] = 4 * i + 2;
    indices[6 * i + 5] = 4 * i + 3;
  }
}


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
  floor_spots.reserve(1000);

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
  tiles_positions.reserve(1000);
  tiles_colors.reserve(1000);
  editor_position.reserve(1);
  editor_color.reserve(1);
  floor_positions.reserve(1000);

  positions_temp.reserve(100);
  colors_temp.reserve(100);
  models_temp.reserve(100);

  AnimatedPosColorTexVertex::init();

  moving_bo.initModels(100);
  static_bo.initModels(100);
  doors_bo.initCubes(1000);
  winning_doors_bo.initCubes(1000);
  tiles_bo.initQuads(1000);
  editor_bo.initCubes(1);
  quads_bo.initQuads(1000);
  floor_bo.initModels(100);
  bg_bo.initModels(1);


  bgfx::ShaderHandle v_animated_tex = Common::loadShader("bin/v_animated_tex.bin");
  bgfx::ShaderHandle f_animated_tex = Common::loadShader("bin/f_animated_tex.bin");
  bgfx::ShaderHandle v_simple = Common::loadShader("bin/v_simple.bin");
  bgfx::ShaderHandle f_simple = Common::loadShader("bin/f_simple.bin");
  bgfx::ShaderHandle f_noise_simple = Common::loadShader("bin/f_noise_simple.bin");
  bgfx::ShaderHandle v_tex = Common::loadShader("bin/v_tex.bin");
  bgfx::ShaderHandle f_tex = Common::loadShader("bin/f_tex.bin");
  bgfx::ShaderHandle f_editor = Common::loadShader("bin/f_editor.bin");
  bgfx::ShaderHandle f_bg = Common::loadShader("bin/f_bg.bin");

  bgfx::ProgramHandle p_animated_tex = bgfx::createProgram(v_animated_tex, f_animated_tex, false);
  bgfx::ProgramHandle p_simple = bgfx::createProgram(v_simple, f_simple, false);
  bgfx::ProgramHandle p_tex = bgfx::createProgram(v_tex, f_tex, false);
  bgfx::ProgramHandle p_noise_simple = bgfx::createProgram(v_simple, f_noise_simple, false);
  bgfx::ProgramHandle p_editor = bgfx::createProgram(v_simple, f_editor, false);
  bgfx::ProgramHandle p_bg = bgfx::createProgram(v_animated_tex, f_bg, false);

  moving_bo.createBuffers();
  moving_bo.m_program = p_animated_tex;
  static_bo.createBuffers();
  static_bo.m_program = p_animated_tex;
  doors_bo.createBuffers();
  doors_bo.m_program = p_simple;
  winning_doors_bo.createBuffers();
  winning_doors_bo.m_program = p_noise_simple;
  tiles_bo.createBuffers();
  tiles_bo.m_program = p_tex;
  editor_bo.createBuffers();
  editor_bo.m_program = p_editor;
  quads_bo.createBuffers();
  quads_bo.m_program = p_tex;
  floor_bo.createBuffers();
  floor_bo.m_program = p_animated_tex;
  bg_bo.createBuffers();
  bg_bo.m_program = p_bg;


  std::vector<std::string> texture_assets = {
    "assets/t1.png",
    "assets/t2.png"
  };
  tiles_bo.textures.prepare(texture_assets);
  moving_bo.textures.prepare(texture_assets);
  static_bo.textures.prepare(texture_assets);



  std::vector<bx::Vec3> vertices;
  std::vector<bx::Vec3> colors;
  std::vector<bx::Vec3> normals;
  std::vector<bx::Vec3> uvs;
  std::vector<int> indices;


  float y = -1.0f;
  float h = 1.5f;
  float lw = 0.05f;

  int planes = 8;
  vertices.resize(planes * 4);
  colors.resize(planes * 4);
  normals.resize(planes * 4);
  uvs.resize(planes * 4);
  indices.resize(planes * 6);

  bx::Vec3 inside_color(0.8f, 0.84f, 0.85f);
  bx::Vec3 border_color_main(0.4f, 0.4f, 0.4f);
  bx::Vec3 gradient_color = bx::sub(bg_color, 0.2);

  fr(i, colors) {
    colors[i] = border_color_main;
    normals[i] = bx::Vec3(0.0f, 1.0f, 0.0f);
    uvs[i] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  }

  // inside
  vertices[ 0] = bx::Vec3( 1.0f, y, -1.0f + lw);
  vertices[ 1] = bx::Vec3( 1.0f, y,  1.0f);
  vertices[ 2] = bx::Vec3(-1.0f + lw, y, -1.0f + lw);
  vertices[ 3] = bx::Vec3(-1.0f + lw, y,  1.0f);
  colors[0] = colors[1] = colors[2] = colors[3] = inside_color;

  // left
  vertices[ 4] = bx::Vec3(-1.0f + lw, y, -1.0f + lw);
  vertices[ 5] = bx::Vec3(-1.0f + lw, y,  1.0f);
  vertices[ 6] = bx::Vec3(-1.0f, y, -1.0f);
  vertices[ 7] = bx::Vec3(-1.0f, y,  1.0f + lw);

  // right
  vertices[ 8] = bx::Vec3(1.0f + lw, y, -1.0f);
  vertices[ 9] = bx::Vec3(1.0f + lw, y,  1.0f + lw);
  vertices[10] = bx::Vec3(1.0f, y, -1.0f + lw);
  vertices[11] = bx::Vec3(1.0f, y,  1.0f);

  // bottom
  vertices[12] = bx::Vec3( 1.0f + lw, y, -1.0f);
  vertices[13] = bx::Vec3( 1.0f, y, -1.0f + lw);
  vertices[14] = bx::Vec3(-1.0f, y, -1.0f);
  vertices[15] = bx::Vec3(-1.0f + lw, y, -1.0f + lw);

  // top
  vertices[16] = bx::Vec3( 1.0f, y, 1.0f);
  vertices[17] = bx::Vec3( 1.0f + lw, y, 1.0f + lw);
  vertices[18] = bx::Vec3(-1.0f + lw, y, 1.0f);
  vertices[19] = bx::Vec3(-1.0f, y, 1.0f + lw);

  // left gradient
  vertices[20] = bx::Vec3(-1.0f, y - h, -1.0f);
  vertices[21] = bx::Vec3(-1.0f, y, -1.0f);
  vertices[22] = bx::Vec3(-1.0f, y - h, 1.0f + lw);
  vertices[23] = bx::Vec3(-1.0f, y, 1.0f + lw);
  colors[21] = colors[23] = gradient_color;
  colors[20] = colors[22] = bg_color;
  normals[20] = normals[21] = normals[22] = normals[23] = bx::Vec3(-1.0f, 0.0f, 0.0f);

  // front gradient
  vertices[24] = bx::Vec3( 1.0f + lw, y - h, -1.0f);
  vertices[25] = bx::Vec3( 1.0f + lw, y, -1.0f);
  vertices[26] = bx::Vec3(-1.0f, y - h, -1.0f);
  vertices[27] = bx::Vec3(-1.0f, y, -1.0f);
  colors[25] = colors[27] = gradient_color;
  colors[24] = colors[26] = bg_color;
  normals[24] = normals[25] = normals[26] = normals[27] = bx::Vec3(0.0f, 0.0f, -1.0f);

  // right gradient
  vertices[28] = bx::Vec3(1.0f + lw, y - h, 1.0f + lw);
  vertices[29] = bx::Vec3(1.0f + lw, y, 1.0f + lw);
  vertices[30] = bx::Vec3(1.0f + lw, y - h, -1.0f);
  vertices[31] = bx::Vec3(1.0f + lw, y, -1.0f);
  colors[29] = colors[31] = gradient_color;
  colors[28] = colors[30] = bg_color;
  normals[28] = normals[29] = normals[30] = normals[31] = bx::Vec3(1.0f, 0.0f, 0.0f);

  for (int i = 0; i < planes; ++i) {
    indices[6 * i + 0] = 4 * i + 1;
    indices[6 * i + 1] = 4 * i + 0;
    indices[6 * i + 2] = 4 * i + 3;
    indices[6 * i + 3] = 4 * i + 0;
    indices[6 * i + 4] = 4 * i + 2;
    indices[6 * i + 5] = 4 * i + 3;
  }

  floor_bo.models.init();
  floor_bo.models.set(vertices, colors, normals, uvs, indices, 0);


  std::vector<bx::Vec3> points;
  points.resize(16);
  lw = 0.0f;
  // top
  points[0] = bx::Vec3( 1.0f + lw,  1.0f + lw, -1.0f);
  points[1] = bx::Vec3( 1.0f + lw,  1.0f + lw,  1.0f + lw);
  points[2] = bx::Vec3(-1.0f,  1.0f + lw, -1.0f);
  points[3] = bx::Vec3(-1.0f,  1.0f + lw,  1.0f + lw);
  // right
  points[4] = bx::Vec3( 1.0f + lw, -1.0f,  1.0f + lw);
  points[5] = bx::Vec3( 1.0f + lw,  1.0f + lw,  1.0f + lw);
  points[6] = bx::Vec3( 1.0f + lw, -1.0f, -1.0f);
  points[7] = bx::Vec3( 1.0f + lw,  1.0f + lw, -1.0f);
  // left
  points[8] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  points[9] = bx::Vec3(-1.0f,  1.0f + lw, -1.0f);
  points[10] = bx::Vec3(-1.0f, -1.0f,  1.0f + lw);
  points[11] = bx::Vec3(-1.0f,  1.0f + lw,  1.0f + lw);
  // front
  points[12] = bx::Vec3( 1.0f + lw, -1.0f, -1.0f);
  points[13] = bx::Vec3( 1.0f + lw,  1.0f + lw, -1.0f);
  points[14] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  points[15] = bx::Vec3(-1.0f,  1.0f + lw, -1.0f);

  gen_quads_with_edges(
      points,
      vertices,
      colors,
      normals,
      uvs,
      indices
      );

  static_bo.models.init();
  // static_bo.models.import("test.obj", 0);
  // static_bo.models.import("test-keyframe2.obj", 1);
  // static_bo.models.import("untitled.obj", 2);
  // static_bo.models.import("cube.obj", 4);
  static_bo.models.set(vertices, colors, normals, uvs, indices, 0);

  // vertices.resize(4);
  // colors.resize(4);
  // normals.resize(4);
  // uvs.resize(4);
  // indices.resize(6);
  //
  // vertices[0] = bx::Vec3( 1.0f, -1.0f, -1.0f);
  // vertices[1] = bx::Vec3( 1.0f, -1.0f,  1.0f);
  // vertices[2] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  // vertices[3] = bx::Vec3(-1.0f, -1.0f,  1.0f);
  //
  // colors[0] = colors[1] = colors[2] = colors[3] = bx::Vec3(0.0f, 0.0f, 0.0f);
  // normals[0] = normals[1] = normals[2] = normals[3] = bx::Vec3(0.0f, 1.0f, 0.0f);
  //
  // uvs[0] = bx::Vec3(moving_bo.textures.mappings[0].x2, moving_bo.textures.mappings[0].y1, 0.0f);
  // uvs[1] = bx::Vec3(moving_bo.textures.mappings[0].x2, moving_bo.textures.mappings[0].y2, 0.0f);
  // uvs[2] = bx::Vec3(moving_bo.textures.mappings[0].x1, moving_bo.textures.mappings[0].y1, 0.0f);
  // uvs[3] = bx::Vec3(moving_bo.textures.mappings[0].x1, moving_bo.textures.mappings[0].y2, 0.0f);
  //
  // // uvs[0] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  // // uvs[1] = bx::Vec3( 1.0f, -1.0f,  1.0f);
  // // uvs[2] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  // // uvs[3] = bx::Vec3(-1.0f, -1.0f,  1.0f);
  //
  // indices[0] = 1;
  // indices[1] = 0;
  // indices[2] = 3;
  // indices[3] = 0;
  // indices[4] = 2;
  // indices[5] = 3;
  //
  // moving_bo.models.init();
  // moving_bo.models.set(vertices, colors, normals, uvs, indices, 0);


  moving_bo.models.init();
  moving_bo.models.import("cube.obj", 0);




  float screen_size = 100.0f;
  float bg_y = -5.0f;
  vertices[0] = bx::Vec3( screen_size, bg_y, -screen_size);
  vertices[1] = bx::Vec3( screen_size, bg_y,  screen_size);
  vertices[2] = bx::Vec3(-screen_size, bg_y, -screen_size);
  vertices[3] = bx::Vec3(-screen_size, bg_y,  screen_size);

  colors[0] = colors[1] = colors[2] = colors[3] = bx::Vec3(0.0f, 0.0f, 0.0f);

  uvs[0] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  uvs[1] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  uvs[2] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  uvs[3] = bx::Vec3(-1.0f, -1.0f, 0.0f);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 3;
  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  bg_bo.models.init();
  bg_bo.models.set(vertices, colors, normals, uvs, indices, 0);

  static_models_list.reserve(1000);
  moving_models_list.resize(100);
  floor_models_list.reserve(100);
  bg_models_list.reserve(1);

  moving_nimate.prepare(this, &moving_bo, &moving_positions, &moving_colors, &moving_models_list);
}


void World::init()
{
  won = false;
  moving_nimate.reset();

  moving_positions.resize(moving_spots.size());
  static_positions.resize(static_spots.size());
  doors_positions.resize(doors_spots.size());
  winning_doors_positions.resize(winning_doors_spots.size());
  tiles_positions.resize(tiles_spots.size());
  editor_position.resize(1);
  moving_colors.resize(moving_spots.size());
  static_colors.resize(static_spots.size());
  doors_colors.resize(doors_spots.size());
  winning_doors_colors.resize(winning_doors_spots.size());
  tiles_colors.resize(tiles_spots.size());
  editor_color.resize(1);
  through_door.resize(moving_spots.size());
  moving_cur_spots.resize(moving_spots.size());
  floor_positions.resize(floor_spots.size());
  floor_colors.resize(floor_spots.size());


  setPositionsFromSpots(moving_positions, moving_spots);
  setPositionsFromSpots(static_positions, static_spots);
  setPositionsFromSpots(doors_positions, doors_spots);
  setPositionsFromSpots(winning_doors_positions, winning_doors_spots);
  setPositionsFromSpots(tiles_positions, tiles_spots);
  setPositionsFromSpots(editor_position, editor_spot);
  setPositionsFromSpots(floor_positions, floor_spots);

  bg_positions.resize(1);
  bg_colors.resize(1);
  bg_positions[0] = bx::Vec3(0.0f, 0.0f, 0.0f);
  bg_colors[0] = bg_color;

  setColors(moving_colors, moving_color);
  setColors(static_colors, static_color);
  for (int i = 0; i < doors_colors.size(); ++i) {
    doors_colors[i] = gate_colors[(int)(i / 2)];
  }
  setColors(winning_doors_colors, winning_doors_color);
  setColors(tiles_colors, tiles_color);
  setColors(editor_color, editor_thing_color);
  setColors(floor_colors, floor_color);


  moving_models_list.resize(moving_positions.size());
  models_temp.resize(moving_models_list.size());
  fr(i, moving_positions) {
    moving_models_list[i] = 0;
    models_temp[i] = moving_models_list[i];
  }

  bg_models_list.resize(1);
  bg_models_list[0] = 0;


  writeModelsVertices(moving_bo, moving_positions, moving_colors, moving_models_list);
  writeModelsVertices(static_bo, static_positions, static_colors, static_models_list);
  writeCubesVertices(doors_bo, doors_positions, doors_colors);
  writeCubesVertices(winning_doors_bo, winning_doors_positions, winning_doors_colors);
  writeFloorVertices(tiles_bo, tiles_positions, tiles_colors, tiles_mapping_ids);
  writeCubesVertices(editor_bo, editor_position, editor_color);
  writeModelsVertices(floor_bo, floor_positions, floor_colors, floor_models_list);
  printf("bg\n");
  writeModelsVertices(bg_bo, bg_positions, bg_colors, bg_models_list);

  moving_next_spots = moving_spots;


  moving_nimate.init();
}


void World::updateBuffers()
{
  moving_bo.updateBuffer();
  static_bo.updateBuffer();
  doors_bo.updateBuffer();
  winning_doors_bo.updateBuffer();
  tiles_bo.updateBuffer();
  floor_bo.updateBuffer();
  bg_bo.updateBuffer();

  quads_bo.updateBuffer();
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
      // models_temp[i] = (models_temp[i] + 1) % 2;
      models_temp[i] = 0;
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
  moving_bo.drawModels(view, BGFX_STATE_BLEND_ALPHA);
  static_bo.drawModels(view, 0);
  doors_bo.drawCubes(view, doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  winning_doors_bo.drawCubes(view, winning_doors_spots.size(), BGFX_STATE_BLEND_ALPHA);
  tiles_bo.drawQuads(view, tiles_spots.size());
  floor_bo.drawModels(view, 0);
  bg_bo.drawModels(view, 0);

  quads_bo.drawQuads(view, quads_count);

  if (in_editor) {
    editor_bo.drawCubes(view, 1, BGFX_STATE_BLEND_ALPHA);
  }
}


void World::destroy()
{
  moving_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();
  floor_bo.destroy();
  bg_bo.destroy();
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

  won = winning_doors_spots.size() > 0 && winning_count == winning_doors_spots.size();

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
  if (positions.empty()) {
    bo.models_vertices_count = 0;
  } else {
    int nth_model_vertices_count = bo.models.nth_model_vertices_count(nth_model);
    int nth_model_indices_count = bo.models.nth_model_indices_count(nth_model);

    for (int i = 0; i < positions.size(); ++i) {
      bo.writeModelVertices(
          nth_model_vertices_count * i,
          positions[i],
          colors[i],
          nth_model
          );
      bo.writeModelIndices(
          nth_model_indices_count * i,
          nth_model_vertices_count * i,
          nth_model
          );
    }
  }
}


void World::writeModelsVertices
(BufferObject& bo, const std::vector<bx::Vec3>& positions, const std::vector<bx::Vec3>& colors, const std::vector<int>& models_list)
{
  if (positions.empty()) {
    bo.models_vertices_count = 0;
  } else {
    int acc_vertices_offset = 0;
    int acc_indices_offset = 0;

    for (int i = 0; i < positions.size(); ++i) {
      bo.writeModelVertices(
          acc_vertices_offset,
          positions[i],
          colors[i],
          models_list[i]
          );
      bo.writeModelIndices(
          acc_indices_offset,
          acc_vertices_offset,
          models_list[i]
          );

      acc_vertices_offset += bo.models.nth_model_vertices_count(models_list[i]);
      acc_indices_offset += bo.models.nth_model_indices_count(models_list[i]);
    }
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
  if (positions1.empty()) {
    bo.models_vertices_count = 0;
  } else {
    int acc_vertices_offset = 0;
    int acc_indices_offset = 0;

    for (int i = 0; i < positions1.size(); ++i) {
      bo.writeModelVertices(
          acc_vertices_offset,
          positions1[i],
          positions2[i],
          colors1[i],
          colors2[i],
          nth1s[i],
          nth2s[i],
          froms[i],
          tos[i]
          );
      bo.writeModelIndices(
          acc_indices_offset,
          acc_vertices_offset,
          nth1s[i]
          );

      acc_vertices_offset += bo.models.nth_model_vertices_count(nth1s[i]);
      acc_indices_offset += bo.models.nth_model_indices_count(nth2s[i]);
    }
  }
}


void World::writeFloorVertices
(BufferObject& bo,
 const std::vector<bx::Vec3>& positions,
 const std::vector<bx::Vec3>& colors,
 const std::vector<int>& mapping_ids
 )
{
  std::vector<bx::Vec3> tiles_vs;
  std::vector<bx::Vec3> tiles_cs;
  int tile_size = 1.0f;
  tiles_vs.resize(mapping_ids.size() * 4);
  tiles_cs.resize(mapping_ids.size() * 4);

  for (int i = 0; i < mapping_ids.size(); ++i) {
    tiles_vs[i * 4 + 0] = bx::Vec3(positions[i].x + tile_size, -1.0f, positions[i].z - tile_size);
    tiles_vs[i * 4 + 1] = bx::Vec3(positions[i].x + tile_size, -1.0f, positions[i].z + tile_size);
    tiles_vs[i * 4 + 2] = bx::Vec3(positions[i].x - tile_size, -1.0f, positions[i].z - tile_size);
    tiles_vs[i * 4 + 3] = bx::Vec3(positions[i].x - tile_size, -1.0f, positions[i].z + tile_size);
    tiles_cs[i * 4 + 0] =
      tiles_cs[i * 4 + 1] =
      tiles_cs[i * 4 + 2] =
      tiles_cs[i * 4 + 3] = colors[i];
  }

  bo.writeQuadsVertices(0, tiles_vs, tiles_cs, mapping_ids);
}
