#include <stdio.h>
#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL_syswm.h>
#include <fstream>
#include <bx/math.h>

#include "buffer_object.hpp"
#include "common.hpp"

SDL_Window* window = NULL;
const int WIDTH = 1600;
const int HEIGHT = 1000;
const int w = WIDTH;
const int h = HEIGHT;
const int w2 = w / 2;
const int h2 = h / 2;

static const int moving_count = 10;
static const int static_count = 10;


BufferObject moving_bo;
BufferObject static_bo;


struct Cube
{
  bx::Vec3 pos {0.0f, 0.0f, 0.0f};
  bx::Vec3 col {0.7f, 0.7f, 0.7f};

  bx::Vec3 spot {0, 0, 0};
};

Cube moving_cubes[moving_count];
Cube static_cubes[static_count];

bx::Vec3 spot_offset = {-5, -5, -5};

void initVertices()
{
  moving_cubes[0].spot = {0, 0, 0};
  moving_cubes[1].spot = {1, 0, 0};
  moving_cubes[2].spot = {2, 0, 0};
  moving_cubes[3].spot = {3, 0, 0};
  moving_cubes[4].spot = {4, 0, 0};
  moving_cubes[5].spot = {5, 0, 0};
  moving_cubes[6].spot = {6, 0, 0};
  moving_cubes[7].spot = {7, 0, 0};
  moving_cubes[8].spot = {8, 0, 0};
  moving_cubes[9].spot = {9, 0, 0};

  static_cubes[0].spot = {5, 0, 0};
  static_cubes[1].spot = {5, 1, 0};
  static_cubes[2].spot = {5, 2, 0};
  static_cubes[3].spot = {5, 3, 0};
  static_cubes[4].spot = {5, 3, 1};
  static_cubes[5].spot = {5, 3, 2};
  static_cubes[6].spot = {5, 3, 3};
  static_cubes[7].spot = {5, 2, 3};
  static_cubes[8].spot = {5, 1, 3};
  static_cubes[9].spot = {5, 0, 3};

  for (int i = 0; i < moving_count; ++i) {
    moving_cubes[i].col = bx::Vec3(0.85f, 0.2f, 0.32f);
    moving_cubes[i].pos = bx::add(bx::mul(moving_cubes[i].spot, 2.0f), spot_offset);
    moving_bo.writeCubeVertices(i, moving_cubes[i].pos, moving_cubes[i].col);
  }

  for (int i = 0; i < static_count; ++i) {
    static_cubes[i].col = bx::Vec3(0.0f, 99/255.0f, 115/255.0f);
    static_cubes[i].pos = bx::add(bx::mul(static_cubes[i].spot, 2.0f), spot_offset);
    static_bo.writeCubeVertices(i, static_cubes[i].pos, static_cubes[i].col);
  }
}

void initShit()
{
  moving_bo.initCubes(moving_count);
  static_bo.initCubes(static_count);

  initVertices();

  moving_bo.createBuffers();
  moving_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
  static_bo.createBuffers();
  static_bo.createShaders("bin/v_simple.bin", "bin/f_simple.bin");
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


  PosColorVertex::init();
  initShit();




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
          case SDLK_ESCAPE:
            quit = true;
            break;

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

          case SDLK_SPACE:
            front = !front;
            break;
        }
      }
    }

    dt = current_time - last_time;

    for (int i = 0; i < moving_count; ++i) {
      moving_cubes[i].spot = bx::add(moving_cubes[i].spot, cur_pos);
      if (moving_cubes[i].spot.x == 1 && moving_cubes[i].spot.y == 1 && moving_cubes[i].spot.z == 1) {
        moving_cubes[i].spot = bx::Vec3(3, 3, 3);
      }
      moving_cubes[i].pos = bx::add(bx::mul(moving_cubes[i].spot, 2.0f), spot_offset);
      moving_bo.writeCubeVertices(i, moving_cubes[i].pos, moving_cubes[i].col);
    }

    moving_bo.updateBuffer();

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

    // Set view 0 default viewport.
    // bgfx::setViewRect(0, 0, 0,
    //                   WIDTH,
    //                   HEIGHT);

    // bgfx::touch(0);

    // x += dt * mx;
    // y += dt * my;
    // if (abs(x) >= 3) {
    //   mx *= -1;
    // }
    // if (abs(y) >= 3) {
    //   my *= -1;
    // }

    // Set render states.
    bgfx::setState(0
        | BGFX_STATE_WRITE_R
        | BGFX_STATE_WRITE_G
        | BGFX_STATE_WRITE_B
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_WRITE_Z
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_CULL_CCW
        | BGFX_STATE_MSAA
        // | BGFX_STATE_PT_LINES
        // | BGFX_STATE_PT_LINESTRIP
        );

    float mtx[16];
    bx::mtxSRT(mtx, 1, 1, 1, 0, 0, 0, 0.0f, 0.0f, 0);

    // Set model matrix for rendering.
    bgfx::setTransform(mtx);

    u_twh_val[0] = current_time;
    bgfx::setUniform(u_twh, &u_twh_val);

    moving_bo.draw();
    static_bo.draw();

    bgfx::frame();

    last_time = current_time;
    current_time = SDL_GetTicks();
  }

  moving_bo.destroy();
  static_bo.destroy();
  bgfx::destroy(u_twh);

  bgfx::shutdown();
  // Free up window
  SDL_DestroyWindow(window);
  // Shutdown SDL
  SDL_Quit();

  return 0;
}
