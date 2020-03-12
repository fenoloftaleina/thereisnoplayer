#ifndef TEXTURES
#define TEXTURES
#pragma once

#include <vector>
#include <string>
#include <bgfx/bgfx.h>
#include <bimg/bimg.h>

struct Textures
{
  struct Mapping {
    float x1, y1, x2, y2;
  };

  const int texture_size = 1024;

  std::vector<Mapping> mappings;

  bgfx::UniformHandle sampler_handle;
  bgfx::TextureHandle texture_handle;

  void prepare(const std::vector<std::string>& resources_pathnames);
  void setTexture();

  bool armed = false;
};

#endif
