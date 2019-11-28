#include <stdio.h>
#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL_syswm.h>
#include <bx/math.h>
#include <vector>
#include <fstream>
#include "cereal/include/cereal/types/vector.hpp"
#include "cereal/include/cereal/archives/portable_binary.hpp"

#include "common.hpp"
#include "objs.hpp"
#include "editor.hpp"
#include "logic.hpp"

SDL_Window* window = NULL;
const int WIDTH = 1600;
const int HEIGHT = 1000;
const int w = WIDTH;
const int h = HEIGHT;
const int w2 = w / 2;
const int h2 = h / 2;

int current_level = 0;
char level_str[100];

Logic logic;
Objs& objs = logic.objs;
Editor editor;

void updateAllVerticesAndBuffers()
{
  objs.init();
}

void load(const char* filename)
{
  std::ifstream is(filename, std::ios::binary);
  cereal::PortableBinaryInputArchive ar(is);
  ar(objs.moving_cubes, objs.static_cubes, objs.doors, objs.winning_doors);
}

void save(const char* filename)
{
  std::ofstream os(filename, std::ios::binary);
  cereal::PortableBinaryOutputArchive ar(os);
  ar(objs.moving_cubes, objs.static_cubes, objs.doors, objs.winning_doors);
}

void runLevel(int level)
{
  current_level = level;
  sprintf(level_str, "levels/s%d", current_level);
  load(level_str);
  for (int i = 0; i < objs.kids_cubes.size(); ++i) {
    objs.kids_cubes[i].kids_moves_offset = 0;
  }
  SDL_SetWindowTitle(window, level_str);

  updateAllVerticesAndBuffers();
}

void saveLevel(int level)
{
  sprintf(level_str, "levels/s%d", level);
  save(level_str);
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


  editor.objs = &objs;
  objs.preInit();
  runLevel(0);

  bgfx::UniformHandle u_twh = bgfx::createUniform("twh", bgfx::UniformType::Vec4);

  bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);
  bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);
  bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
  bgfx::setViewClear(0,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x555555FF, 1.0f, 0);
                     // 0x443355FF, 1.0f, 0);

  // bgfx::touch(0);

  float u_twh_val[4];
  u_twh_val[1] = w;
  u_twh_val[2] = h;

  bx::Vec3 cur_pos(0.0f, 0.0f, 0.0f);
  bx::Vec3 cur_pos2(0.0f, 0.0f, 0.0f);
  float mx = 1.0f, my = 1.0f;

  bool in_editor = false;

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
          case SDLK_a:
          case SDLK_LEFT:
            cur_pos.x -= mx;
            break;

          case SDLK_d:
          case SDLK_RIGHT:
            cur_pos.x += mx;
            break;

          case SDLK_w:
          case SDLK_UP:
            cur_pos.z += mx;
            break;

          case SDLK_s:
          case SDLK_DOWN:
            cur_pos.z -= mx;
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
            in_editor = !in_editor;
            break;

          case SDLK_ESCAPE:
            in_editor = false;
            break;

          case SDLK_u:
            // moving/user
            if (!in_editor) break;
            editor.addMovingCube(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_i:
            // static
            if (!in_editor) break;
            editor.addStaticCube(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_o:
            // winnning
            if (!in_editor) break;
            editor.addWinningDoor(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_j:
            // gate
            if (!in_editor) break;
            editor.addOrUpdateDoor(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_k:
            // kids
            if (!in_editor) break;
            editor.addKidsCube(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_n:
            // remote/no
            if (!in_editor) break;
            editor.removeOnSpot(objs.editor_cube.spot);
            updateAllVerticesAndBuffers();
            break;

          case SDLK_p:
            // persist
            if (!in_editor) break;
            saveLevel(current_level);
            break;

          case SDLK_m:
            // save level on next slot
            if (!in_editor) break;
            saveLevel(current_level + 1);
            runLevel(current_level + 1);
            break;
        }
      }
    }

    dt = current_time - last_time;

    if (logic.run(cur_pos, in_editor)) {
      runLevel(++current_level);
    }

    const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
    // const bx::Vec3 at = bx::neg(Common::spot_offset);

    // const bx::Vec3 eye = { -5.0f, 2.0f, -10.0f };
    // const bx::Vec3 eye = { 5.0f, 2.0f, -10.0f };
    // bx::Vec3 eye = {  -10.0f, 25.0f, -10.0f };
    bx::Vec3 eye = {  0.0f, 15.0f, -10.0f };

    // Set view and projection matrix for view 0.
    float view[16];
    bx::mtxLookAt(view, eye, at);

    float proj[16];

    bx::mtxProj(proj,
        70.0f,
        float(WIDTH)/float(HEIGHT),
        0.1f, 100.0f,
        bgfx::getCaps()->homogeneousDepth);

    // float size = 30.0f;
    // bx::mtxOrtho(proj, -size, size, -size, size, 0.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    // float mtx[16];
    // bx::mtxSRT(mtx, 1, 1, 1, 0, 45, 0, 0.0f, 0.0f, 0);
    // bgfx::setTransform(mtx);

    u_twh_val[0] = current_time;
    bgfx::setUniform(u_twh, &u_twh_val);

    objs.draw(in_editor);
    bgfx::frame();

    last_time = current_time;
    current_time = SDL_GetTicks();
  }

  objs.destroy();
  bgfx::destroy(u_twh);

  bgfx::shutdown();
  // Free up window
  SDL_DestroyWindow(window);
  // Shutdown SDL
  SDL_Quit();

  return 0;
}
