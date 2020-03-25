#include "textures.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

void Textures::prepare(const std::vector<std::string>& resources_pathnames)
{
  sampler_handle = bgfx::createUniform("smplr",  bgfx::UniformType::Sampler);

  int width, height, nrChannels;
  unsigned char* image;
  unsigned char* texture = new unsigned char[texture_size * texture_size * 4]{};

  stbi_set_flip_vertically_on_load(true);
  mappings.reserve(resources_pathnames.size());

  int i = 0, j = 0, max_line_height = 0;

  for(auto& pathname : resources_pathnames) {
    image = stbi_load(pathname.c_str(), &width, &height, &nrChannels, 0);

    for(int m = 0; m < height; ++m) {
      memcpy(texture + i * 4 + j * texture_size * 4 + m * texture_size * 4, image + m * width * 4, width * 4);
    }

    Textures::Mapping mapping{};
    mapping.x1 = i / (float)texture_size;
    mapping.y1 = j / (float)texture_size;
    mapping.x2 = (i + width) / (float)texture_size;
    mapping.y2 = (j + height) / (float)texture_size;

    mappings.push_back(mapping);

    if (height > max_line_height) {
      max_line_height = height;
    }

    i += width + 1;
    if (i >= 1024) {
      i = 0;
      j = max_line_height + 1;
    }

    stbi_image_free(image);
  }

  const bgfx::Memory *mem = bgfx::makeRef(texture, texture_size * texture_size * 4);
  texture_handle = bgfx::createTexture2D(
      texture_size,
      texture_size,
      false,
      1,
      bgfx::TextureFormat::BGRA8,
      0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP,
      mem
      );

  armed = true;
}

void Textures::setTexture()
{
  if (armed) {
    bgfx::setTexture(0, sampler_handle, texture_handle);
  }
}
