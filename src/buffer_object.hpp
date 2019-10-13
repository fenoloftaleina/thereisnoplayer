#ifndef BUFFER_OBJECT
#define BUFFER_OBJECT
#pragma once

#include <stdio.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include "common.hpp"


struct PosColorVertex {
  float x;
  float y;
  float z;
  float r;
  float g;
  float b;
  float normal_x;
  float normal_y;
  float normal_z;

  static void init() {
    ms_layout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,   3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
      .end();
  };

  static bgfx::VertexLayout ms_layout;
};


static const int faces_per_cube_count = 6;
static const int vertices_per_face_count = 4;
static const int vertices_per_cube_count = faces_per_cube_count * vertices_per_face_count;
static const int indices_per_face_count = 6;
static const int indices_per_cube_count = indices_per_face_count * faces_per_cube_count;

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


struct BufferObject {
  int vertices_count;
  int indices_count;

  PosColorVertex* vertices;
  uint16_t* indices;

  bgfx::DynamicVertexBufferHandle m_vbh;
  bgfx::IndexBufferHandle m_ibh;
  bgfx::ProgramHandle m_program;


  void initCubes(const int cubes_count);
  void writeCubesIndices();
  void writeCubeVertices(const int nth_cube, bx::Vec3 pos, bx::Vec3 col);
  void setFaceColor(const int nth_cube, const int nth_face, bx::Vec3 col);
  void createBuffers();
  void updateBuffer();
  void createShaders(const char* vertex_shader_path, const char* fragment_shader_path);
  void draw(uint16_t current_vertices_count, uint16_t current_indices_count);
  void drawCubes(uint16_t current_cubes_count);
  void destroy();
};

#endif
