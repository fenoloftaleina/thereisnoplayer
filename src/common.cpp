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

bool Common::sameSpot(const bx::Vec3& v1, const bx::Vec3 v2)
{
  return (int)(v1.x) == (int)(v2.x) && (int)(v1.y) == (int)(v2.y) && (int)(v1.z) == (int)(v2.z);
}

bx::Vec3 Common::posOnSpot(const bx::Vec3& spot)
{
  static const bx::Vec3 constexpr spot_offset = {-5, -5, -5};

  return bx::add(bx::mul(spot, 2.0f), spot_offset);
}

#include <sys/time.h>

void Common::tim(const char* info)
{
  static long long prev_tim;

  struct timeval te;
  gettimeofday(&te, NULL);
  long long now = te.tv_sec * 1000LL + te.tv_usec / 1000;

  if (prev_tim == 0) {
    prev_tim = now;
  } else {
    long long diff = now - prev_tim;
    if (diff > 20) {
      printf("---> %s elapsed: %lld, %lld %lld\n", info, now - prev_tim, now, prev_tim);
    }
    prev_tim = 0;
  }
}
