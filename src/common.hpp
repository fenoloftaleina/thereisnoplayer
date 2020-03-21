#ifndef COMMON
#define COMMON
#pragma once

#include <stdio.h>
#include <fstream>
#include <vector>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include "../cereal/include/cereal/types/vector.hpp"
#include "../cereal/include/cereal/archives/portable_binary.hpp"

struct Common
{
  static void pv3(bx::Vec3 v);
  static bgfx::ShaderHandle loadShader(const char* _name);
  static bool sameSpot(const bx::Vec3& v1, const bx::Vec3 v2);
  static bx::Vec3 posOnSpot(const bx::Vec3& spot);
  static void tim(const char* info);
};

#endif
