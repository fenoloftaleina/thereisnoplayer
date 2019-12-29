#ifndef MODELS
#define MODELS
#pragma once

#include "common.hpp"
#include <bx/math.h>
#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BufferObject;
struct PosColorVertex;

struct Models
{
  const int vertices_count = 10000;
  const int indices_count = 10000;
  PosColorVertex* vertices;
  uint16_t* indices;

  uint16_t vertices_offsets[100];
  uint16_t indices_offsets[100];

  void init();
  void import(const char* filename, const int nth);
  void put(const int nth, BufferObject& bo);

  int processNode(aiNode* node, const aiScene* scene, int& vertices_offset, int& indices_offset);
  void processMesh(aiMesh *mesh, const aiScene *scene, int& vertices_offset, int& indices_offset);

  char filename_str[255];
};

#include "buffer_object.hpp"

#endif
