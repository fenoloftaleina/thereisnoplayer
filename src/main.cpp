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

#include "common.hpp"
#include "world.hpp"
#include "editor.hpp"

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

  world.init();
  world.updateBuffers();
}

void persistLevel(int level_id)
{
  sprintf(level_str, "levels/%s", levels[level_id].filename.c_str());
  save(level_str);
}



struct D
{
  bx::Vec3 p;
  int i;
  float f1, f2, f3;

  template<class Archive>
    void serialize(Archive& archive)
    {
      archive(p, i, f1, f2, f3);
    }
};

struct Lvl
{
  std::vector<bx::Vec3> m;
  std::vector<bx::Vec3> s;
  std::vector<D> d;
  std::vector<D> wd;

  template<class Archive>
    void serialize(Archive& archive)
    {
      archive(m, s, d, wd);
    }
};

int main (int argc, char* args[])
{
  std::vector<Lvl> lvls;

  std::ifstream is("lvls", std::ios::binary);
  cereal::JSONInputArchive ar(is);
  ar(lvls);

  fr(i, lvls) {
    world.moving_spots.clear();
    fr(j, lvls[i].m) {
      world.moving_spots.push_back(
          {(int)(lvls[i].m[j].x), (int)(lvls[i].m[j].z)}
          );
    }

    world.static_spots.clear();
    fr(j, lvls[i].s) {
      world.static_spots.push_back(
          {(int)(lvls[i].s[j].x), (int)(lvls[i].s[j].z)}
          );
    }

    world.doors_spots.clear();
    fr(j, lvls[i].d) {
      world.doors_spots.push_back(
          {(int)(lvls[i].d[j].p.x), (int)(lvls[i].d[j].p.z)}
          );
    }

    world.winning_doors_spots.clear();
    fr(j, lvls[i].wd) {
      world.winning_doors_spots.push_back(
          {(int)(lvls[i].wd[j].p.x), (int)(lvls[i].wd[j].p.z)}
          );
    }

    sprintf(level_str, "levels/level%d", i);
    save(level_str);
  }

  return 0;


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

  Spot move;

  bool in_editor = false;
  bool back = false;

  // Poll for events and wait till user closes window
  bool quit = false;
  uint32_t last_time = SDL_GetTicks(), current_time = SDL_GetTicks(), dt;
  SDL_Event currentEvent;
  while(!quit) {
    move.x = move.y = 0;
    back = false;

    while(SDL_PollEvent(&currentEvent)) {
      if(currentEvent.type == SDL_QUIT) {
        quit = true;
      } else if (currentEvent.type == SDL_KEYDOWN) {
        switch (currentEvent.key.keysym.sym) {
          case SDLK_a:
          case SDLK_LEFT:
            move.x = -1;
            break;

          case SDLK_d:
          case SDLK_RIGHT:
            move.x = 1;
            break;

          case SDLK_w:
          case SDLK_UP:
            move.y = 1;
            break;

          case SDLK_s:
          case SDLK_DOWN:
            move.y = -1;
            break;

          case SDLK_r:
            world.reset();
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

    // if (logic.run(cur_pos, in_editor, back, dt)) {
    //   runLevel(current_level_id + 1);
    // }

    world.resolve(move, in_editor, back);
    world.update(dt);

    const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
    // const bx::Vec3 at = bx::neg(Common::spot_offset);

    // const bx::Vec3 eye = { -5.0f, 2.0f, -10.0f };
    // const bx::Vec3 eye = { 5.0f, 2.0f, -10.0f };
    // bx::Vec3 eye = {  -10.0f, 25.0f, -10.0f };
    bx::Vec3 eye = {  0.0f, 25.0f, -10.0f };

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

    world.draw(in_editor);
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
