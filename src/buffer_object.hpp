#ifndef BUFFER_OBJECT
#define BUFFER_OBJECT
#pragma once

#include <stdio.h>
#include <vector>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include "common.hpp"
#include "textures.hpp"
#include "models.hpp"


static const int faces_per_cube_count = 6;
static const int vertices_per_face_count = 4;
static const int vertices_per_cube_count = faces_per_cube_count * vertices_per_face_count;
static const int indices_per_face_count = 6;
static const int indices_per_cube_count = indices_per_face_count * faces_per_cube_count;

static const int vertices_per_lines_face_count = 4;
static const int vertices_per_lines_cube_count = faces_per_cube_count * vertices_per_lines_face_count;
static const int indices_per_lines_face_count = 6;
static const int indices_per_lines_cube_count = faces_per_cube_count * indices_per_lines_face_count;

static const bx::Vec3 constexpr pos_vertices[vertices_per_cube_count] = {
  // CCW culling, indices: 0, 1, 3, 1, 2, 3, means: 'u' shapes starting on the right

  // front
  {  1.0f,  1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},

  // right
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f,  1.0f, -1.0f,},

  // back
  { -1.0f,  1.0f,  1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f,  1.0f,  1.0f,},

  // left
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //top
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //bottom
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f,  1.0f,},
};

static const bx::Vec3 constexpr pos_lines_vertices[vertices_per_cube_count] = {
  // CCW culling, indices: 0, 1, 3, 1, 2, 3, means: 'u' shapes starting on the right

  // front
  {  1.0f,  1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},

  // right
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f,  1.0f, -1.0f,},

  // back
  { -1.0f,  1.0f,  1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f,  1.0f,  1.0f,},

  // left
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //top
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //bottom
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f,  1.0f,},
};


struct BufferObject {
  int vertices_count;
  int indices_count;

  AnimatedPosColorTexVertex* vertices;
  uint16_t* indices;

  bgfx::DynamicVertexBufferHandle m_vbh;
  bgfx::DynamicIndexBufferHandle m_ibh;
  bgfx::ProgramHandle m_program;

  Textures textures;
  Models models;

  void prepare(const std::vector<std::string>& texture_assets);
  void initCubes(const int cubes_count);
  void initCubesLines(const int cubes_count);
  void initModels(const int models_count);
  void initQuads(const int quads_count);
  void writeCubesIndices();
  void writeCubesLinesIndices();
  void writeCubeVertices(const int nth_cube, bx::Vec3 pos, bx::Vec3 col);
  void writeCubeLinesVertices(const int nth_cube, bx::Vec3 pos, bx::Vec3 col);
  void writeModelVertices(const int offset, bx::Vec3 pos, bx::Vec3 col, const int nth);
  void writeModelVertices
    (const int offset, const bx::Vec3 pos1, const bx::Vec3 pos2,
     const bx::Vec3 col1, const bx::Vec3 col2,
     const int nth1, const int nth2, const bx::Vec3 from, const bx::Vec3 to);
  void writeModelIndices(const int offset, const int vertices_num_offset, const int nth);
  void writeQuadsVertices(const int offset, const std::vector<bx::Vec3>& vs, const std::vector<bx::Vec3>cs, const std::vector<int>& mapping_ids);
  void writeQuadsIndices();
  void setFaceColor(const int nth_cube, const int nth_face, bx::Vec3 col);
  void createBuffers();
  void updateBuffer();
  void createShaders(const char* vertex_shader_path, const char* fragment_shader_path);
  void draw(bgfx::ViewId view, uint16_t current_vertices_count, uint16_t current_indices_count, uint64_t more_state);
  void drawCubes(bgfx::ViewId view, uint16_t current_cubes_count, uint64_t more_state = 0);
  void drawCubesLines(bgfx::ViewId view, uint16_t current_cubes_count);
  void drawModels(bgfx::ViewId view, uint64_t more_state);
  void drawQuads(bgfx::ViewId view, uint16_t current_quads_count, uint64_t more_state = 0);
  void destroy();

  int models_vertices_count = 0;
  int models_indices_count = 0;

  int offset, mapping_id;
  bx::Vec3 end_pos, normal, a, b, c;
};


#endif
