#include "buffer_object.hpp"

bgfx::VertexLayout AnimatedPosColorVertex::ms_layout;


void BufferObject::initCubes(const int cubes_count)
{
  vertices_count = cubes_count * vertices_per_cube_count;
  indices_count = cubes_count * indices_per_lines_cube_count;

  vertices = new AnimatedPosColorVertex[vertices_count];
  indices = new uint16_t[indices_count];

  writeCubesIndices();
}


void BufferObject::initCubesLines(const int cubes_count)
{
  vertices_count = cubes_count * vertices_per_lines_cube_count;
  indices_count = cubes_count * vertices_per_lines_cube_count;

  vertices = new AnimatedPosColorVertex[vertices_count];
  indices = new uint16_t[indices_count];

  writeCubesLinesIndices();
}


void BufferObject::writeCubesIndices()
{
  for (int i = 0; i < indices_count / indices_per_face_count; ++i) {
    indices[i * indices_per_face_count + 0] = i * vertices_per_face_count + 0;
    indices[i * indices_per_face_count + 1] = i * vertices_per_face_count + 1;
    indices[i * indices_per_face_count + 2] = i * vertices_per_face_count + 3;
    indices[i * indices_per_face_count + 3] = i * vertices_per_face_count + 1;
    indices[i * indices_per_face_count + 4] = i * vertices_per_face_count + 2;
    indices[i * indices_per_face_count + 5] = i * vertices_per_face_count + 3;
  }
}


void BufferObject::writeCubesLinesIndices()
{
  for (int i = 0; i < indices_count / indices_per_lines_face_count; ++i) {
    indices[i * indices_per_lines_face_count + 0] = i * vertices_per_lines_face_count + 0;
    indices[i * indices_per_lines_face_count + 1] = i * vertices_per_lines_face_count + 1;
    indices[i * indices_per_lines_face_count + 2] = i * vertices_per_lines_face_count + 0;
    indices[i * indices_per_lines_face_count + 3] = i * vertices_per_lines_face_count + 3;
    indices[i * indices_per_lines_face_count + 4] = i * vertices_per_lines_face_count + 1;
    indices[i * indices_per_lines_face_count + 5] = i * vertices_per_lines_face_count + 2;
  }
}


void BufferObject::writeCubeVertices(int nth_cube, bx::Vec3 pos, bx::Vec3 col)
{
  bx::Vec3 end_pos, normal, a, b, c;
  int offset = nth_cube * vertices_per_cube_count;

  for (int i = 0; i < vertices_per_cube_count; ++i) {
    end_pos = bx::add(pos_vertices[i], pos);

    vertices[offset + i].x = end_pos.x;
    vertices[offset + i].y = end_pos.y;
    vertices[offset + i].z = end_pos.z;
    vertices[offset + i].r = col.x;
    vertices[offset + i].g = col.y;
    vertices[offset + i].b = col.z;
  }

  for(int i = 0; i < vertices_per_cube_count; i += 4) {
    a = bx::Vec3(
        vertices[offset + i + 0].x,
        vertices[offset + i + 0].y,
        vertices[offset + i + 0].z
        );
    b = bx::Vec3(
        vertices[offset + i + 1].x,
        vertices[offset + i + 1].y,
        vertices[offset + i + 1].z
        );
    c = bx::Vec3(
        vertices[offset + i + 3].x,
        vertices[offset + i + 3].y,
        vertices[offset + i + 3].z
        );

    normal = bx::normalize(
        bx::cross(
          bx::sub(a, b),
          bx::sub(a, c)
          )
        );

    vertices[offset + i + 0].normal_x =
      vertices[offset + i + 1].normal_x =
      vertices[offset + i + 2].normal_x =
      vertices[offset + i + 3].normal_x =
      normal.x;

    vertices[offset + i + 0].normal_y =
      vertices[offset + i + 1].normal_y =
      vertices[offset + i + 2].normal_y =
      vertices[offset + i + 3].normal_y =
      normal.y;

    vertices[offset + i + 0].normal_z =
      vertices[offset + i + 1].normal_z =
      vertices[offset + i + 2].normal_z =
      vertices[offset + i + 3].normal_z =
      normal.z;
  }
}

void BufferObject::writeCubeLinesVertices(int nth_cube, bx::Vec3 pos, bx::Vec3 col)
{
  bx::Vec3 end_pos;
  int offset = nth_cube * vertices_per_lines_cube_count;

  for (int i = 0; i < vertices_per_lines_cube_count; ++i) {
    end_pos = bx::add(pos_lines_vertices[i], pos);

    vertices[offset + i].x = end_pos.x;
    vertices[offset + i].y = end_pos.y;
    vertices[offset + i].z = end_pos.z;
    vertices[offset + i].r = col.x;
    vertices[offset + i].g = col.y;
    vertices[offset + i].b = col.z;
  }
}

void BufferObject::setFaceColor(const int nth_cube, const int nth_face, bx::Vec3 col)
{
  int offset = nth_cube * vertices_per_cube_count;
  int face_offset = nth_face * vertices_per_face_count;

  for (int i = 0; i < vertices_per_face_count; ++i) {
    vertices[offset + face_offset + i].r = col.x;
    vertices[offset + face_offset + i].g = col.y;
    vertices[offset + face_offset + i].b = col.z;
  }
}

void BufferObject::createBuffers()
{
  m_vbh = bgfx::createDynamicVertexBuffer(
              // Static data can be passed with bgfx::makeRef
              bgfx::makeRef(vertices, vertices_count * sizeof(vertices[0])),
              AnimatedPosColorVertex::ms_layout
          );

  m_ibh = bgfx::createDynamicIndexBuffer(
              // Static data can be passed with bgfx::makeRef
              bgfx::makeRef(indices, indices_count * sizeof(indices[0]))
              // bgfx::makeRef(indices, 6 * sizeof(indices[0]))
          );
}

void BufferObject::updateBuffer()
{
  bgfx::update(m_vbh, 0, bgfx::makeRef(vertices, vertices_count * sizeof(vertices[0])));
  bgfx::update(m_ibh, 0, bgfx::makeRef(indices, indices_count * sizeof(indices[0])));
}

void BufferObject::createShaders(const char* vertex_shader_path, const char* fragment_shader_path)
{
  bgfx::ShaderHandle vsh = Common::loadShader(vertex_shader_path);
  bgfx::ShaderHandle fsh = Common::loadShader(fragment_shader_path);

  m_program = bgfx::createProgram(vsh, fsh, true);
}

void BufferObject::draw(uint16_t current_vertices_count, uint16_t current_indices_count, uint64_t more_state = 0)
{
  bgfx::setState(0
      | BGFX_STATE_WRITE_R
      | BGFX_STATE_WRITE_G
      | BGFX_STATE_WRITE_B
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_WRITE_Z
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_CULL_CCW
      | BGFX_STATE_MSAA
      | more_state
      );

  // printf("\nvertices:\n");
  // for(int i = 0; i < current_vertices_count; ++i) {
  //   printf("%f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
  // }
  // printf("\nindices:\n");
  // for(int i = 0; i < current_indices_count; ++i) {
  //   printf("%d, ", indices[i]);
  // }
  // printf("\n\n");

  bgfx::setVertexBuffer(0, m_vbh, 0, current_vertices_count);
  bgfx::setIndexBuffer(m_ibh, 0, current_indices_count);

  bgfx::submit(0, m_program);
}

void BufferObject::drawCubes(uint16_t current_cubes_count, uint64_t more_state)
{
  draw(current_cubes_count * vertices_per_cube_count, current_cubes_count * indices_per_cube_count, more_state);
}

void BufferObject::drawCubesLines(uint16_t current_cubes_count)
{
  draw(current_cubes_count * vertices_per_lines_cube_count, current_cubes_count * indices_per_lines_cube_count, BGFX_STATE_PT_LINES);
}

void BufferObject::destroy()
{
  bgfx::destroy(m_vbh);
  bgfx::destroy(m_ibh);
  bgfx::destroy(m_program);
}



void BufferObject::initModels(const int models_count)
{
  vertices_count = models_count * 10000;
  indices_count = models_count * 10000;

  vertices = new AnimatedPosColorVertex[vertices_count];
  indices = new uint16_t[indices_count];
}


void BufferObject::drawModels
(const int models_count, const Models& models, const int nth, uint64_t more_state = 0)
{
  draw(
      models.nth_model_vertices_count(nth) * models_count,
      models.nth_model_indices_count(nth) * models_count,
      more_state
      );
}


void BufferObject::drawModels(uint64_t more_state = 0)
{
  draw(models_vertices_count, models_indices_count, more_state);
}


void BufferObject::writeModelVertices
(const int offset, bx::Vec3 pos, bx::Vec3 col, const Models& models, const int nth)
{
  int nth_model_vertices_count = models.nth_model_vertices_count(nth);

  for (int i = 0; i < nth_model_vertices_count; ++i) {
    vertices[offset + i].x =
      models.vertices[models.vertices_offsets[nth] + i].x;
    vertices[offset + i].y =
      models.vertices[models.vertices_offsets[nth] + i].y;
    vertices[offset + i].z =
      models.vertices[models.vertices_offsets[nth] + i].z;
    vertices[offset + i].r = col.x;
    vertices[offset + i].g = col.y;
    vertices[offset + i].b = col.z;
    vertices[offset + i].normal_x =
      models.vertices[models.vertices_offsets[nth] + i].normal_x;
    vertices[offset + i].normal_y =
      models.vertices[models.vertices_offsets[nth] + i].normal_y;
    vertices[offset + i].normal_z =
      models.vertices[models.vertices_offsets[nth] + i].normal_z;

    vertices[offset + i].pos_x1 = pos.x;
    vertices[offset + i].pos_y1 = pos.y;
    vertices[offset + i].pos_z1 = pos.z;

    vertices[offset + i].pos_x2 = pos.x;
    vertices[offset + i].pos_y2 = pos.y;
    vertices[offset + i].pos_z2 = pos.z;

    vertices[offset + i].pos_from = 0;
    vertices[offset + i].pos_to = 0;
  }

  // if (nth_model_vertices_count + offset > models_vertices_count) {
  models_vertices_count = nth_model_vertices_count + offset;
}


void BufferObject::writeModelVertices
(const int offset, const bx::Vec3 pos1, const bx::Vec3 pos2,
 const bx::Vec3 col1, const bx::Vec3 col2, const Models& models,
 const int nth1, const int nth2, const bx::Vec3 from, const bx::Vec3 to)
{
  int nth_model_vertices_count = models.nth_model_vertices_count(nth1);

  for (int i = 0; i < nth_model_vertices_count; ++i) {
    vertices[offset + i].x = models.vertices[models.vertices_offsets[nth1] + i].x;
    vertices[offset + i].y = models.vertices[models.vertices_offsets[nth1] + i].y;
    vertices[offset + i].z = models.vertices[models.vertices_offsets[nth1] + i].z;
    vertices[offset + i].r = col1.x;
    vertices[offset + i].g = col1.y;
    vertices[offset + i].b = col1.z;
    vertices[offset + i].normal_x = models.vertices[models.vertices_offsets[nth1] + i].normal_x;
    vertices[offset + i].normal_y = models.vertices[models.vertices_offsets[nth1] + i].normal_y;
    vertices[offset + i].normal_z = models.vertices[models.vertices_offsets[nth1] + i].normal_z;

    vertices[offset + i].pos_x1 = pos1.x;
    vertices[offset + i].pos_y1 = pos1.y;
    vertices[offset + i].pos_z1 = pos1.z;

    vertices[offset + i].pos_x2 = pos2.x;
    vertices[offset + i].pos_y2 = pos2.y;
    vertices[offset + i].pos_z2 = pos2.z;

    vertices[offset + i].pos_from = from.z;
    vertices[offset + i].pos_to = to.z;
  }

  // if (nth_model_vertices_count + offset > models_vertices_count) {
  models_vertices_count = nth_model_vertices_count + offset;
}


void BufferObject::writeModelIndices
(const int offset, const int vertices_num_offset, const Models& models, const int nth)
{
  int nth_model_indices_count = models.nth_model_indices_count(nth);

  for (int i = 0; i < nth_model_indices_count; ++i) {
    indices[offset + i] = vertices_num_offset + models.indices[models.indices_offsets[nth] + i];
  }

  // if (nth_model_indices_count + offset > models_indices_count) {
  models_indices_count = nth_model_indices_count + offset;
}
