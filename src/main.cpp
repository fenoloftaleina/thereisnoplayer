#include <stdio.h>
#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL_syswm.h>
#include <fstream>
#include <bx/math.h>
#include <vector>
#include "cereal/include/cereal/types/vector.hpp"
#include "cereal/include/cereal/archives/portable_binary.hpp"

#include "buffer_object.hpp"
#include "common.hpp"

SDL_Window* window = NULL;
const int WIDTH = 1600;
const int HEIGHT = 1000;
const int w = WIDTH;
const int h = HEIGHT;
const int w2 = w / 2;
const int h2 = h / 2;

const int cubes_in_memory_count = 300;
const int doors_in_memory_count = 30;

int current_level = 0;
const char* levels[] = {
  "levels/intro1",
  "levels/intro2",
  "levels/intro3",
  "levels/intro4",
  "levels/intro5",
  "levels/a",
  "levels/second",
  "levels/nothing",
};


BufferObject moving_bo;
BufferObject static_bo;
BufferObject doors_bo;
BufferObject winning_doors_bo;
BufferObject editor_bo;

struct Cube
{
  bx::Vec3 pos {0.0f, 0.0f, 0.0f};
  bx::Vec3 col {0.7f, 0.7f, 0.7f};

  bx::Vec3 spot {0, 0, 0};
  bx::Vec3 next_spot {0, 0, 0};

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(spot.x, spot.y, spot.z);
  }
};

std::vector<Cube> moving_cubes;
std::vector<Cube> static_cubes;
Cube editor_cube;

bx::Vec3 spot_offset = {-5, -5, -5};

struct Door
{
  Cube cube;
  int towards;
  bx::Vec3 collision_face_normal;

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(cube, towards, collision_face_normal.x, collision_face_normal.y, collision_face_normal.z);
  }
};

std::vector<Door> doors;
std::vector<Door> winning_doors;

void load(const char* filename)
{
  std::ifstream is(filename, std::ios::binary);
  cereal::PortableBinaryInputArchive ar(is);
  ar(moving_cubes, static_cubes, doors, winning_doors);
}

void save(const char* filename)
{
  std::ofstream os(filename, std::ios::binary);
  cereal::PortableBinaryOutputArchive ar(os);
  ar(moving_cubes, static_cubes, doors, winning_doors);
}

bx::Vec3 posOnSpot(const bx::Vec3& spot)
{
  return bx::add(bx::mul(spot, 2.0f), spot_offset);
}

bx::Vec3 moving_cubes_color = {0.85f, 0.2f, 0.32f};
bx::Vec3 static_cubes_color = {0.0f, 99/255.0f, 115/255.0f};
bx::Vec3 editor_cube_color = {0.3f, 0.3f, 0.3f};
bx::Vec3 doors_color = {0.1f, 99/255.0f, 15/255.0f};
bx::Vec3 winning_doors_color = {0.5f, 0.5f, 0.5f};
bx::Vec3 gate_colors[] = {
  {0.7f, 0.1f, 0.5f},
  {0.2f, 1.0f, 0.1f},
  {0.5f, 0.2f, 0.3f},
  {0.1f, 0.1f, 0.1f},
  {0.5f, 0.5f, 0.5f},
  {0.7f, 0.9f, 0.9f},
};

void initVertices()
{
  // moving_cubes.resize(10);
  // moving_cubes[0].spot = {0, 0, 0};
  // moving_cubes[1].spot = {1, 0, 0};
  // moving_cubes[2].spot = {2, 0, 0};
  // moving_cubes[3].spot = {3, 0, 0};
  // moving_cubes[4].spot = {4, 0, 0};
  // moving_cubes[5].spot = {5, 0, 0};
  // moving_cubes[6].spot = {6, 0, 0};
  // moving_cubes[7].spot = {7, 0, 0};
  // moving_cubes[8].spot = {8, 0, 0};
  // moving_cubes[9].spot = {9, 0, 0};
  //
  // static_cubes.resize(10);
  // static_cubes[0].spot = {5, 5, 0};
  // static_cubes[1].spot = {5, 1, 0};
  // static_cubes[2].spot = {5, 2, 0};
  // static_cubes[3].spot = {5, 3, 0};
  // static_cubes[4].spot = {5, 3, 1};
  // static_cubes[5].spot = {5, 3, 2};
  // static_cubes[6].spot = {5, 3, 3};
  // static_cubes[7].spot = {5, 2, 3};
  // static_cubes[8].spot = {5, 1, 3};
  // static_cubes[9].spot = {5, 1, 3};
  //
  // doors.resize(2);
  // doors[0].cube.spot = {1, 1, 1};
  // doors[0].towards = 1;
  // doors[0].collision_face_normal = {0, 0, -1};
  // doors[1].cube.spot = {3, 3, 3};
  // doors[1].towards = -1;
  // doors[1].collision_face_normal = {-1, 0, 0};
  //
  // winning_doors.resize(2);
  // winning_doors[0].cube.spot = {-2, 5, 7};
  // winning_doors[1].cube.spot = {5, 5, 7};
  //
  // save("levels/nothing");

  editor_cube.col = editor_cube_color;
  editor_cube.pos = bx::add(bx::mul(editor_cube.spot, 2.0f), spot_offset);
  editor_bo.writeCubeVertices(0, editor_cube.pos, editor_cube.col);

  for (int i = 0; i < moving_cubes.size(); ++i) {
    moving_cubes[i].col = moving_cubes_color;
    moving_cubes[i].pos = posOnSpot(moving_cubes[i].spot);
    moving_bo.writeCubeVertices(i, moving_cubes[i].pos, moving_cubes[i].col);
  }

  for (int i = 0; i < static_cubes.size(); ++i) {
    static_cubes[i].col = static_cubes_color;
    static_cubes[i].pos = posOnSpot(static_cubes[i].spot);
    static_bo.writeCubeVertices(i, static_cubes[i].pos, static_cubes[i].col);
  }

  int door_face;
  for (int i = 0; i < doors.size(); ++i) {
    doors[i].cube.col = doors_color;
    doors[i].cube.pos = posOnSpot(doors[i].cube.spot);
    doors_bo.writeCubeVertices(i, doors[i].cube.pos, doors[i].cube.col);

    if (doors[i].collision_face_normal.x) {
      door_face = 3;
    } else if(doors[i].collision_face_normal.y) {
      door_face = 4;
    } else {
      door_face = 0;
    }
    doors_bo.setFaceColor(i, door_face, gate_colors[i - (doors[i].towards == -1)]);
  }

  for (int i = 0; i < winning_doors.size(); ++i) {
    winning_doors[i].cube.col = winning_doors_color;
    winning_doors[i].cube.pos = posOnSpot(winning_doors[i].cube.spot);
    winning_doors_bo.writeCubeVertices(i, winning_doors[i].cube.pos, winning_doors[i].cube.col);
  }
}

int idxForSpot(const std::vector<Cube>& v, const bx::Vec3& spot)
{
  for (int i = 0; i < v.size(); ++i) {
    if (Common::sameSpot(v[i].spot, spot)) {
      return i;
    }
  }

  return -1;
}

int idxForSpot(const std::vector<Door>& v, const bx::Vec3& spot)
{
  for (int i = 0; i < v.size(); ++i) {
    if (Common::sameSpot(v[i].cube.spot, spot)) {
      return i;
    }
  }

  return -1;
}

bool removeOnIdx(std::vector<Cube>& v, int i)
{
  if (i != -1) {
    v.erase(v.begin() + i);
    return true;
  }

  return false;
}

bool removeOnIdx(std::vector<Door>& v, int i)
{
  if (i != -1) {
    v.erase(v.begin() + i);
    return true;
  }

  return false;
}

void removeOnSpot(const bx::Vec3& spot)
{
  removeOnIdx(moving_cubes, idxForSpot(moving_cubes, spot)) ||
    removeOnIdx(static_cubes, idxForSpot(static_cubes, spot)) ||
    removeOnIdx(doors, idxForSpot(doors, spot)) ||
    removeOnIdx(winning_doors, idxForSpot(winning_doors, spot));
}

void addMovingCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = moving_cubes_color;
  c.spot = spot;
  c.pos = posOnSpot(c.spot);
  moving_cubes.push_back(c);
}

void addStaticCube(const bx::Vec3& spot)
{
  Cube c;
  c.col = static_cubes_color;
  c.spot = spot;
  c.pos = posOnSpot(c.spot);
  static_cubes.push_back(c);
}

void addOrUpdateDoor(const bx::Vec3& spot, const bx::Vec3& collision_face_normal)
{
  Door d;
  d.cube.col = doors_color;
  d.cube.spot = spot;
  d.cube.pos = posOnSpot(d.cube.spot);
  d.collision_face_normal = collision_face_normal;
  int idx = idxForSpot(doors, spot);
  if (idx == -1) {
    d.towards = doors.back().towards * -1;
    doors.push_back(d);
  } else {
    d.towards = doors[idx].towards;
    doors[idx] = d;
  }
}


void addWinningDoor(const bx::Vec3& spot)
{
  Door d;
  d.cube.col = winning_doors_color;
  d.cube.spot = spot;
  d.cube.pos = posOnSpot(d.cube.spot);
  winning_doors.push_back(d);
}

void initBos()
{
  moving_bo.initCubes(cubes_in_memory_count);
  static_bo.initCubes(cubes_in_memory_count);
  doors_bo.initCubes(doors_in_memory_count);
  winning_doors_bo.initCubes(doors_in_memory_count);
  editor_bo.initCubes(1);

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  doors_bo.createBuffers();
  doors_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  winning_doors_bo.createBuffers();
  winning_doors_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
  editor_bo.createBuffers();
  editor_bo.createShaders("bin/v_simple.bin", "bin/f_noise_simple.bin");
}

void runLevel(int level)
{
  current_level = level;
  load(levels[current_level]);

  initVertices();
}

void updateAllVerticesAndBuffers()
{
  initVertices();

  moving_bo.updateBuffer();
  static_bo.updateBuffer();
  doors_bo.updateBuffer();
  winning_doors_bo.updateBuffer();
}

int main (int argc, char* args[])
{
  // Initialize SDL systems
  if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n",
           SDL_GetError());
  }
  else {
    //Create a window
    window = SDL_CreateWindow("TITLE TITLE",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if( window == NULL ) {
      printf("Window could not be created! SDL_Error: %s\n",
             SDL_GetError());
    }
  }

  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi)) {
    return 1;
  }

  bgfx::PlatformData pd;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  pd.ndt = wmi.info.x11.display;
#	elif BX_PLATFORM_OSX
  pd.ndt = NULL;
#	elif BX_PLATFORM_WINDOWS
  pd.ndt = NULL;
#	elif BX_PLATFORM_STEAMLINK
  pd.ndt = wmi.info.vivante.display;
#	endif // BX_PLATFORM_

#	if BX_PLATFORM_LINUX || wX_PLATFORM_BSD
  pd.nwh = (void*)wmi.info.x11.window;
#	elif BX_PLATFORM_OSX
  pd.nwh = wmi.info.cocoa.window;
#	elif BX_PLATFORM_WINDOWS
  pd.nwh = wmi.info.win.window;
#	elif BX_PLATFORM_STEAMLINK
  pd.nwh =  wmi.info.vivante.window;
#	endif // BX_PLATFORM_

  //
  // // Tell bgfx about the platform and window
  bgfx::setPlatformData(pd);

  // Render an empty frame
  bgfx::renderFrame();

  // Initialize bgfx
  bgfx::init();
  // bgfx::init(bgfx::RendererType::Metal);


  moving_cubes.reserve(cubes_in_memory_count);
  static_cubes.reserve(cubes_in_memory_count);
  doors.reserve(doors_in_memory_count);
  winning_doors.reserve(doors_in_memory_count);
  editor_cube.spot = {0, 0, 0};
  PosColorVertex::init();
  initBos();
  runLevel(0);



  bgfx::UniformHandle u_twh = bgfx::createUniform("twh", bgfx::UniformType::Vec4);



  // Reset window
  bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);

  // Enable debug text.
  bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

  // Set view rectangle for 0th view
  bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));

  // Clear the view rect
  bgfx::setViewClear(0,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x555555FF, 1.0f, 0);
                     // 0x443355FF, 1.0f, 0);


  // Set empty primitive on screen
  // bgfx::touch(0);

  float u_twh_val[4];
  u_twh_val[1] = w;
  u_twh_val[2] = h;

  bx::Vec3 cur_pos(0.0f, 0.0f, 0.0f);
  float mx = 1.0f, my = 1.0f;

  bool front = true;
  bool were_collisions;
  int winning_count;
  bool editor = false;

  // Poll for events and wait till user closes window
  bool quit = false;
  uint32_t last_time = SDL_GetTicks(), current_time = SDL_GetTicks(), dt;
  SDL_Event currentEvent;
  while(!quit) {
    cur_pos.x = cur_pos.y = cur_pos.z = 0.0f;

    while(SDL_PollEvent(&currentEvent)) {
      if(currentEvent.type == SDL_QUIT) {
        quit = true;
      } else if (currentEvent.type == SDL_KEYDOWN) {
        switch (currentEvent.key.keysym.sym) {
          case SDLK_q:
            if (front) {
              cur_pos.z += mx;
            } else {
              cur_pos.x -= mx;
            }
            break;

          case SDLK_d:
            if (front) {
              cur_pos.z -= mx;
            } else {
              cur_pos.x += mx;
            }
            break;

          case SDLK_e:
            if (front) {
              cur_pos.x += mx;
            } else {
              cur_pos.z += mx;
            }
            break;

          case SDLK_a:
            if (front) {
              cur_pos.x -= mx;
            } else {
              cur_pos.z -= mx;
            }
            break;

          case SDLK_w:
            cur_pos.y += my;
            break;

          case SDLK_s:
            cur_pos.y -= my;
            break;

          case SDLK_f:
            // front = !front;
            break;

          case SDLK_r:
            runLevel(current_level);
            break;

          case SDLK_v:
            runLevel(--current_level);
            break;

          case SDLK_b:
            runLevel(++current_level);
            break;

          case SDLK_h:
            editor = !editor;
            break;

          case SDLK_u:
            // moving/user
            addMovingCube(editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_i:
            // static
            addStaticCube(editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_o:
            // winnning
            addWinningDoor(editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_j:
            // gate left
            addOrUpdateDoor(editor_cube.spot, bx::Vec3(-1, 0, 0));
            updateAllVerticesAndBuffers();
            break;

          case SDLK_k:
            // gate top
            addOrUpdateDoor(editor_cube.spot, bx::Vec3(0, 1, 0));
            updateAllVerticesAndBuffers();
            break;

          case SDLK_l:
            // gate right
            addOrUpdateDoor(editor_cube.spot, bx::Vec3(0, 0, -1));
            updateAllVerticesAndBuffers();
            break;

          case SDLK_n:
            // remote/no
            removeOnSpot(editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_p:
            // persist
            save(levels[current_level]);
            break;
        }
      }
    }

    dt = current_time - last_time;

    winning_count = 0;
    for (int i = 0; i < moving_cubes.size(); ++i) {
      for (int j = 0; j < winning_doors.size(); ++j) {
        if (Common::sameSpot(moving_cubes[i].spot, winning_doors[j].cube.spot)) {
          winning_count += 1;
        }
      }
    }

    if (winning_count == winning_doors.size() && winning_doors.size() > 0) {
      runLevel(++current_level);
    }

    if (!editor) {
      were_collisions = false;
      for (int i = 0; i < moving_cubes.size(); ++i) {
        moving_cubes[i].next_spot = bx::add(moving_cubes[i].spot, cur_pos);

        for (int j = 0; j < doors.size(); ++j) {
          if (Common::sameSpot(moving_cubes[i].next_spot, doors[j].cube.spot)) {
            if (Common::sameSpot(moving_cubes[i].spot, bx::add(doors[j].cube.spot, doors[j].collision_face_normal))) {
              moving_cubes[i].next_spot = bx::add(doors[j + doors[j].towards].cube.spot, doors[j + doors[j].towards].collision_face_normal);
            } else {
              were_collisions = true;
            }
          }
        }
      }
      for (int i = 0; i < moving_cubes.size(); ++i) {
        for (int j = i + 1; j < moving_cubes.size(); ++j) {
          were_collisions = were_collisions || Common::sameSpot(moving_cubes[i].next_spot, moving_cubes[j].next_spot);
        }

        for (int j = 0; j < static_cubes.size(); ++j) {
          were_collisions = were_collisions || Common::sameSpot(moving_cubes[i].next_spot, static_cubes[j].spot);
        }
      }

      if (!were_collisions) {
        for (int i = 0; i < moving_cubes.size(); ++i) {
          moving_cubes[i].spot = moving_cubes[i].next_spot;

          moving_cubes[i].pos = bx::add(bx::mul(moving_cubes[i].spot, 2.0f), spot_offset);
          moving_bo.writeCubeVertices(i, moving_cubes[i].pos, moving_cubes[i].col);
        }

        moving_bo.updateBuffer();
      } else {
        printf("COLLISIONS!\n");
      }
    } else {
      // editor

      editor_cube.spot = bx::add(editor_cube.spot, cur_pos);
      editor_cube.pos = bx::add(bx::mul(editor_cube.spot, 2.0f), spot_offset);
      editor_bo.writeCubeVertices(0, editor_cube.pos, editor_cube.col);
      editor_bo.updateBuffer();
    }

    const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
    // const bx::Vec3 eye = { -5.0f, 2.0f, -10.0f };
    // const bx::Vec3 eye = { 5.0f, 2.0f, -10.0f };
    bx::Vec3 eye = {  -15.0f, 15.0f, -15.0f };
    if (!front) {
      eye.x *= -1;
    }

    // Set view and projection matrix for view 0.
    float view[16];
    bx::mtxLookAt(view, eye, at);

    float proj[16];
    bx::mtxProj(proj,
        60.0f,
        float(WIDTH)/float(HEIGHT),
        0.1f, 100.0f,
        bgfx::getCaps()->homogeneousDepth);

    // bx::mtxOrtho(proj, 0.0f, w, h, 0.0f, -10.1f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    float mtx[16];
    bx::mtxSRT(mtx, 1, 1, 1, 0, 0, 0, 0.0f, 0.0f, 0);

    // Set model matrix for rendering.
    bgfx::setTransform(mtx);

    u_twh_val[0] = current_time;
    bgfx::setUniform(u_twh, &u_twh_val);

    moving_bo.drawCubes(moving_cubes.size());
    static_bo.drawCubes(static_cubes.size());
    doors_bo.drawCubes(doors.size());
    winning_doors_bo.drawCubes(winning_doors.size());

    if (editor) {
      editor_bo.drawCubes(1);
    }

    bgfx::frame();

    last_time = current_time;
    current_time = SDL_GetTicks();
  }

  moving_bo.destroy();
  static_bo.destroy();
  doors_bo.destroy();
  winning_doors_bo.destroy();
  editor_bo.destroy();
  bgfx::destroy(u_twh);

  bgfx::shutdown();
  // Free up window
  SDL_DestroyWindow(window);
  // Shutdown SDL
  SDL_Quit();

  return 0;
}
