#include "common.hpp"

void Common::pv3(bx::Vec3 v)
{
  printf("%f %f %f\n", v.x, v.y, v.z);
}

bgfx::ShaderHandle Common::loadShader(const char* _name)
{
  char* data = new char[2048];
  std::ifstream file;
  size_t fileSize;
  file.open(_name);
  assert(file.is_open());

  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read(data, fileSize);
  file.close();

  const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);
  mem->data[mem->size-1] = '\0';
  bgfx::ShaderHandle handle = bgfx::createShader(mem);
  bgfx::setName(handle, _name);
  return handle;
}
