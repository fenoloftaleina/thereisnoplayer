#ifndef COMMON
#define COMMON
#pragma once

#include <stdio.h>
#include <fstream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

namespace Common
{
  void pv3(bx::Vec3 v);
  bgfx::ShaderHandle loadShader(const char* _name);
  bool sameSpot(const bx::Vec3& v1, const bx::Vec3 v2);
};

#endif
