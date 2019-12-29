#ifndef PORTER
#define PORTER
#pragma once

#include "common.hpp"
#include <bx/math.h>
#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct BufferObject;

struct Porter
{
  void import(const char* filename, BufferObject& bo);

  char filename_str[255];
};

#include "buffer_object.hpp"

#endif
