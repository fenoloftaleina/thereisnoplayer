#include <stdio.h>
#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL_syswm.h>
#include <bx/math.h>
#include <vector>
#include <fstream>
#include "../cereal/include/cereal/types/vector.hpp"
#include "../cereal/include/cereal/archives/json.hpp"
#include "../cereal/include/cereal/archives/portable_binary.hpp"
#include <string>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <bimg/bimg.h>

#include "common.hpp"
#include "world.hpp"
#include "editor.hpp"
#include "buffer_object.hpp"

SDL_Window* window = NULL;
const int WIDTH = 1600;
const int HEIGHT = 1000;
const int w = WIDTH;
const int h = HEIGHT;

struct Level
{
  std::string filename;
  std::string note;

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(filename, note);
  }
};


int current_level_id = 0;
char level_str[255];
Level new_level;

std::vector<Level> levels;

World world;
Editor editor;


namespace cereal
{
  template<class Archive>
  void serialize(Archive& archive,
      bx::Vec3& m)
  {
    archive(m.x, m.y, m.z);
  }

  template<class Archive>
  void serialize(Archive& archive,
      Spot& m)
  {
    archive(m.x, m.y);
  }
}

void loadLevels()
{
  std::ifstream is("levels/levels_list", std::ios::binary);
  cereal::JSONInputArchive ar(is);
  ar(levels);
}

void saveLevels()
{
  std::ofstream os("levels/levels_list", std::ios::binary);
  cereal::JSONOutputArchive ar(os);
  ar(levels);
}

void load(const char* filename)
{
  std::ifstream is(filename, std::ios::binary);
  cereal::JSONInputArchive ar(is);
  ar(world);
}

void save(const char* filename)
{
  std::ofstream os(filename, std::ios::binary);
  cereal::JSONOutputArchive ar(os);
  ar(world);
}

void runLevel(int level_id)
{
  current_level_id = level_id;
  sprintf(level_str, "levels/%s", levels[current_level_id].filename.c_str());
  load(level_str);
  SDL_SetWindowTitle(window, level_str);

  world.all_moving_spots.clear();
  world.init();
  world.updateBuffers();
}

void persistLevel(int level_id)
{
  sprintf(level_str, "levels/%s", levels[level_id].filename.c_str());
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



  editor.world = &world;
  world.prepare();

  loadLevels();
  runLevel(0);



  bgfx::UniformHandle u_twh = bgfx::createUniform("twh", bgfx::UniformType::Vec4);

  // for (int i = 0; i < 10; ++i) {
    printf("Caps: %d!\n", bgfx::getCaps()->formats[BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA]);
    printf("Caps: %d!\n", bgfx::getCaps()->formats[BGFX_CAPS_FORMAT_TEXTURE_MSAA]);
    printf("msaa? %d\n", BGFX_CAPS_FORMAT_TEXTURE_MSAA);
  // }
  // return 0;

  bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X16);
  bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);


  bgfx::Attachment gbufferAt[3];
	bgfx::TextureHandle m_gbufferTex[3];

  uint32_t msaa = (BGFX_RESET_VSYNC&BGFX_RESET_MSAA_MASK)>>BGFX_RESET_MSAA_SHIFT;

  const uint64_t tsFlags = 0
    // | (uint64_t(msaa + 1) << BGFX_TEXTURE_RT_MSAA_SHIFT)
    // | BGFX_TEXTURE_MSAA_SAMPLE
    | BGFX_TEXTURE_RT_MSAA_X16
    | BGFX_SAMPLER_MIN_POINT
    | BGFX_SAMPLER_MAG_POINT
    | BGFX_SAMPLER_MIP_POINT
    | BGFX_SAMPLER_U_CLAMP
    | BGFX_SAMPLER_V_CLAMP
    ;

  m_gbufferTex[0] = bgfx::createTexture2D(uint16_t(WIDTH), uint16_t(HEIGHT), false, 1, bgfx::TextureFormat::BGRA8, tsFlags);
  m_gbufferTex[1] = bgfx::createTexture2D(uint16_t(WIDTH), uint16_t(HEIGHT), false, 1, bgfx::TextureFormat::BGRA8, tsFlags);
  m_gbufferTex[2] = bgfx::createTexture2D(uint16_t(WIDTH), uint16_t(HEIGHT), false, 1, bgfx::TextureFormat::D24S8, tsFlags);
  gbufferAt[0].init(m_gbufferTex[0]);
  gbufferAt[1].init(m_gbufferTex[1]);
  gbufferAt[2].init(m_gbufferTex[2]);

	bgfx::TextureHandle texture_handles[2];
  texture_handles[0] = bgfx::createTexture2D(
      uint16_t(WIDTH),
      uint16_t(HEIGHT),
      false,
      1,
      bgfx::TextureFormat::BGRA8,
      // 0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP
      0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_TEXTURE_BLIT_DST
      // (uint64_t(msaa + 1) << BGFX_TEXTURE_RT_MSAA_SHIFT) | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_TEXTURE_BLIT_DST
      );
  texture_handles[1] = bgfx::createTexture2D(
      uint16_t(WIDTH),
      uint16_t(HEIGHT),
      false,
      1,
      bgfx::TextureFormat::BGRA8,
      // 0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP
      0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_TEXTURE_BLIT_DST
      // (uint64_t(msaa + 1) << BGFX_TEXTURE_RT_MSAA_SHIFT) | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP | BGFX_TEXTURE_BLIT_DST
      );

	bgfx::FrameBufferHandle framebuffer_handle;
  // framebuffer_handle = bgfx::createFrameBuffer(BX_COUNTOF(texture_handles), texture_handles);
  // framebuffer_handle = bgfx::createFrameBuffer(1, texture_handles);

  framebuffer_handle = bgfx::createFrameBuffer(BX_COUNTOF(gbufferAt), gbufferAt, true);

  int width, height, nrChannels;
  unsigned char* image;
  stbi_set_flip_vertically_on_load(true);
  image = stbi_load("assets/tex.png", &width, &height, &nrChannels, 0);
  const bgfx::Memory *mem = bgfx::makeRef(image, 4194304);
  bgfx::TextureHandle tex = bgfx::createTexture2D(
      width,
      height,
      false,
      1,
      bgfx::TextureFormat::RGBA8,
      0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP,
      mem
      );
  // printf("AAAAAAAA: %d %d %d\n", 4194304, (float)4194304 / (width * height), width * height * sizeof(image[0]));

	bgfx::UniformHandle sampler_handle;
  sampler_handle = bgfx::createUniform("smplr",  bgfx::UniformType::Sampler);

  bgfx::ViewId main_view = 1;
  bgfx::ViewId deferred_view = 0;


  world.view = deferred_view;

  bgfx::setViewRect(main_view, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
  bgfx::setViewClear(main_view,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x333333FF, 1.0f, 0);
                     // 0x443355FF, 1.0f, 0);
  bgfx::setViewFrameBuffer(main_view, BGFX_INVALID_HANDLE);

  bgfx::setViewRect(deferred_view, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));
  bgfx::setViewClear(deferred_view,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x555555FF, 1.0f, 0);
                     // 0x443355FF, 1.0f, 0);
  bgfx::setViewFrameBuffer(deferred_view, framebuffer_handle);
  bgfx::touch(deferred_view);
  bgfx::touch(main_view);

  BufferObject main_bo;
  main_bo.initQuads(1);
  main_bo.createBuffers();
  main_bo.createShaders("bin/v_fullscreen_texture.bin", "bin/f_fullscreen_texture.bin");
  std::vector<bx::Vec3> quad_vs;
  std::vector<bx::Vec3> quad_cs;
  quad_vs.resize(4);
  quad_cs.resize(4);

  quad_vs[0] = bx::Vec3( 1.0f, -1.0f,  0.0f);
  quad_vs[1] = bx::Vec3( 1.0f,  1.0f,  0.0f);
  quad_vs[2] = bx::Vec3(-1.0f, -1.0f,  0.0f);
  quad_vs[3] = bx::Vec3(-1.0f,  1.0f,  0.0f);

  quad_cs[0] = bx::Vec3(0.0f, 0.0f, 0.0f);
  quad_cs[1] = bx::Vec3(0.0f, 0.0f, 0.0f);
  quad_cs[2] = bx::Vec3(0.0f, 0.0f, 0.0f);
  quad_cs[3] = bx::Vec3(0.0f, 0.0f, 0.0f);

  main_bo.writeQuadsVertices(0, quad_vs, quad_cs);




  float u_twh_val[4];
  u_twh_val[1] = w;
  u_twh_val[2] = h;

  float view[16];
  float proj[16];
  float proj2[16];
  const bgfx::Caps* caps = bgfx::getCaps();

  Spot move;

  bool in_editor = false;
  bool back = false;
  bool reset = false;
  bool moved = false;

  // Poll for events and wait till user closes window
  bool quit = false;
  uint32_t last_time = SDL_GetTicks(), current_time = SDL_GetTicks(), dt;
  SDL_Event currentEvent;
  while(!quit) {
    move.x = 0;
    move.y = 0;
    moved = false;
    back = false;
    reset = false;

    while(SDL_PollEvent(&currentEvent)) {
      if(currentEvent.type == SDL_QUIT) {
        quit = true;
      } else if (currentEvent.type == SDL_KEYDOWN) {
        switch (currentEvent.key.keysym.sym) {
          case SDLK_a:
          case SDLK_LEFT:
            if (moved) break;
            moved = true;
            move.x = -1;
            break;

          case SDLK_d:
          case SDLK_RIGHT:
            if (moved) break;
            moved = true;
            move.x = 1;
            break;

          case SDLK_w:
          case SDLK_UP:
            if (moved) break;
            moved = true;
            move.y = 1;
            break;

          case SDLK_s:
          case SDLK_DOWN:
            if (moved) break;
            moved = true;
            move.y = -1;
            break;

          case SDLK_r:
            if (world.all_moving_spots.empty()) break;
            reset = true;
            break;

          case SDLK_z:
            if (world.all_moving_spots.empty()) break;
            back = true;
            break;

          case SDLK_v:
            runLevel(current_level_id - 1);
            break;

          case SDLK_b:
            runLevel(current_level_id + 1);
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
            editor.add(world.moving_spots, world.editor_spot[0], world.moving_bo);
            break;

          case SDLK_i:
            // static
            if (!in_editor) break;
            editor.add(world.static_spots, world.editor_spot[0], world.static_bo);
            break;

          case SDLK_o:
            // winnning
            if (!in_editor) break;
            editor.add(world.winning_doors_spots, world.editor_spot[0], world.winning_doors_bo);
            break;

          case SDLK_j:
            // gate
            if (!in_editor) break;
            editor.add(world.doors_spots, world.editor_spot[0], world.doors_bo);
            break;

          case SDLK_n:
            // remote/no
            if (!in_editor) break;
            editor.remove(world.editor_spot[0]);
            break;

          case SDLK_p:
            // persist
            if (!in_editor) break;
            persistLevel(current_level_id);
            break;

          case SDLK_k:
            // klone level on next slot
            if (!in_editor) break;
            time_t rawtime;
            time(&rawtime);
            sprintf(level_str, "%s-%s", levels[current_level_id].filename.c_str(), ctime(&rawtime));
            new_level.note = "";
            new_level.filename = level_str;
            levels.insert(levels.begin() + current_level_id + 1, new_level);
            persistLevel(current_level_id + 1);
            saveLevels();
            runLevel(current_level_id + 1);
            break;

          case SDLK_m:
            // move level to the end
            if (!in_editor) break;
            levels.push_back(levels[current_level_id]);
            levels.erase(levels.begin() + current_level_id);
            saveLevels();
            runLevel(levels.size() - 1);
            break;

          case SDLK_l:
            // lose level
            if (!in_editor) break;
            levels.erase(levels.begin() + current_level_id);
            saveLevels();
            runLevel(current_level_id);
            break;

          case SDLK_9:
            // move level back
            if (!in_editor) break;
            levels.insert(levels.begin() + current_level_id - 1, levels[current_level_id]);
            levels.erase(levels.begin() + current_level_id + 1);
            saveLevels();
            runLevel(current_level_id - 1);
            break;

          case SDLK_0:
            // move level forward
            if (!in_editor) break;
            levels.insert(levels.begin() + current_level_id + 2, levels[current_level_id]);
            levels.erase(levels.begin() + current_level_id);
            saveLevels();
            runLevel(current_level_id + 1);
            break;
        }
      }
    }

    dt = current_time - last_time;

    world.resolve(move, in_editor, back, reset);
    if (world.won) {
      runLevel(current_level_id + 1);
    }

    world.update(current_time, dt);

    const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
    // const bx::Vec3 at = bx::neg(Common::spot_offset);

    // const bx::Vec3 eye = { -5.0f, 2.0f, -10.0f };
    // const bx::Vec3 eye = { 5.0f, 2.0f, -10.0f };
    // bx::Vec3 eye = {  -10.0f, 25.0f, -10.0f };
    bx::Vec3 eye = {  0.0f, 25.0f, -10.0f };

    // Set view and projection matrix for view 0.
    bx::mtxLookAt(view, eye, at);

    bx::mtxProj(proj,
        70.0f,
        float(WIDTH)/float(HEIGHT),
        0.1f, 100.0f,
        bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(deferred_view, view, proj);

    // float mtx[16];
    // bx::mtxSRT(mtx, 1, 1, 1, 0, 45, 0, 0.0f, 0.0f, 0);
    // bgfx::setTransform(mtx);


    bx::mtxOrtho(proj2, -1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 100.0f, 0.0f, caps->homogeneousDepth);

    bgfx::setViewTransform(main_view, NULL, proj2);

    u_twh_val[0] = current_time;
    bgfx::setUniform(u_twh, &u_twh_val);

    world.draw(in_editor);

    // bgfx::setTexture(0, sampler_handle, tex);

    bgfx::blit(deferred_view, texture_handles[1], 0, 0, m_gbufferTex[0], 0, 0);

    // bgfx::setTexture(0, sampler_handle, m_gbufferTex[0]);
    bgfx::setTexture(0, sampler_handle, texture_handles[1]);
    main_bo.drawQuads(main_view, 1);

    bgfx::frame();

    last_time = current_time;
    current_time = SDL_GetTicks();
  }

  world.destroy();
  bgfx::destroy(u_twh);

  bgfx::shutdown();
  // Free up window
  SDL_DestroyWindow(window);
  // Shutdown SDL
  SDL_Quit();

  return 0;
}
