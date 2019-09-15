#include <stdio.h>
#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL_syswm.h>
#include <fstream>
#include <bx/math.h>


SDL_Window* window = NULL;
const int WIDTH = 1600;
const int HEIGHT = 1000;
const int w = WIDTH;
const int h = HEIGHT;
const int w2 = w / 2;
const int h2 = h / 2;

bgfx::ShaderHandle loadShader(const char* _name) {
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

struct PosColorVertex {
  float m_x;
  float m_y;
  float m_z;
  float r;
  float g;
  float b;

  static void init() {
    ms_layout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,   3, bgfx::AttribType::Float)
      .end();
  };

  static bgfx::VertexLayout ms_layout;
};


bgfx::VertexLayout PosColorVertex::ms_layout;

static const int cubes_count = 17;
static const int faces_count = cubes_count * 6;
static const int triangles_count = faces_count * 2;
static const int vertices_count = triangles_count * 3;
static const int lines_per_face = 12;
static const int lines_count = faces_count * lines_per_face;
static PosColorVertex* s_cubeVertices = new PosColorVertex[vertices_count];

static uint16_t* s_cubeLineList = new uint16_t[lines_count];

void initLineList()
{
  int lineAccumulator = 0;
  for(int i = 0; i < lines_count; i += lines_per_face) {
    s_cubeLineList[i + 0] = 0 + lineAccumulator;
    s_cubeLineList[i + 1] = 1 + lineAccumulator;
    s_cubeLineList[i + 2] = 0 + lineAccumulator;
    s_cubeLineList[i + 3] = 3 + lineAccumulator;
    s_cubeLineList[i + 4] = 1 + lineAccumulator;
    s_cubeLineList[i + 5] = 3 + lineAccumulator;
    s_cubeLineList[i + 6] = 1 + lineAccumulator;
    s_cubeLineList[i + 7] = 2 + lineAccumulator;
    s_cubeLineList[i + 8] = 1 + lineAccumulator;
    s_cubeLineList[i + 9] = 3 + lineAccumulator;
    s_cubeLineList[i + 10] = 2 + lineAccumulator;
    s_cubeLineList[i + 11] = 3 + lineAccumulator;

    lineAccumulator += 4;
  }
}
      // {
      // 0, 1,
      // 0, 3,
      // 1, 3,
      // 1, 2,
      // 1, 3,
      // 2, 3,
      //
      // 4, 5,
      // 4, 7,
      // 5, 7,
      // 5, 6,
      // 5, 7,
      // 6, 7,
      // };


// static const uint16_t s_cubeLineStrip[] =
//   {
//    0,1,3,
//    1,2,3
//   };

struct v3
{
  float x, y, z;
};

void set6(int i, float x, float y, float z, float r, float g, float b)
{
  s_cubeVertices[i].m_x = x;
  s_cubeVertices[i].m_y = y;
  s_cubeVertices[i].m_z = z;
  s_cubeVertices[i].r = r;
  s_cubeVertices[i].g = g;
  s_cubeVertices[i].b = b;
}

struct C
{
  v3 v_lfu = {-0.5f,  0.5f, 0.5f};
  v3 v_lfd = {-0.5f,  -0.5f, 0.5f};
  v3 v_lbu = {-0.5f,  0.5f, -0.5f};
  v3 v_lbd = {-0.5f,  -0.5f, -0.5f};
  v3 v_rfu = {0.5f,  0.5f, 0.5f};
  v3 v_rfd = {0.5f,  -0.5f, 0.5f};
  v3 v_rbu = {0.5f,  0.5f, -0.5f};
  v3 v_rbd = {0.5f,  -0.5f, -0.5f};

  v3 c_lfu = {-0.5f,  0.5f, 0.5f};
  v3 c_lfd = {0.6f, 0.3f, 0.8f};
  v3 c_lbu = {-0.5f,  0.5f, -0.5f};
  v3 c_lbd = {-0.5f,  -0.5f, -0.5f};
  v3 c_rfu = {0.5f, 0.5f, 0.5f};
  v3 c_rfd = {0.2f, 0.7f, 0.5f};
  v3 c_rbu = {0.5f,  0.5f, -0.5f};
  v3 c_rbd = {0.5f,  -0.5f, -0.5f};

  v3 p = {0.0f, 0.0f, 0.0f};
  v3 s = {1.0f, 1.0f, 1.0f};

  void set2v3(int i, v3 v, v3 c)
  {
    s_cubeVertices[i].m_x = (v.x + p.x) * s.x;
    s_cubeVertices[i].m_y = (v.y + p.y) * s.y;
    s_cubeVertices[i].m_z = (v.z + p.z) * s.z;
    s_cubeVertices[i].r = c.x;
    s_cubeVertices[i].g = c.y;
    s_cubeVertices[i].b = c.z;
  }

  void WriteBuffer(int offset)
  {
    set2v3(offset + 0, v_lfu, c_lfu);
    set2v3(offset + 1, v_lfd, c_lfd);
    set2v3(offset + 2, v_lbd, c_lbd);
    set2v3(offset + 3, v_lbu, c_lbu);

    set2v3(offset + 4, v_rfu, c_rfu);
    set2v3(offset + 5, v_rfd, c_rfd);
    set2v3(offset + 6, v_lfd, c_lfd);
    set2v3(offset + 7, v_lfu, c_lfu);

    set2v3(offset + 8, v_rfu, c_rfu);
    set2v3(offset + 9, v_rbu, c_rbu);
    set2v3(offset + 10, v_rbd, c_rbd);
    set2v3(offset + 11, v_rfd, c_rfd);

    set2v3(offset + 12, v_lbu, c_lbu);
    set2v3(offset + 13, v_rbu, c_rbu);
    set2v3(offset + 14, v_rbd, c_rbd);
    set2v3(offset + 15, v_lbd, c_lbd);

    set2v3(offset + 16, v_lbu, c_rfu);
    set2v3(offset + 17, v_rbu, c_rbu);
    set2v3(offset + 18, v_rfu, c_rbd);
    set2v3(offset + 19, v_lfu, c_rfd);

    set2v3(offset + 20, v_lbd, c_lbd);
    set2v3(offset + 21, v_rbd, c_rbd);
    set2v3(offset + 22, v_rfd, c_rfd);
    set2v3(offset + 23, v_lfd, c_lfd);

    // set6(0, 0.5f,  0.5f, 0.0f, 0.5f, 0.5f, 0.5f);
    // set6(1, 0.5f, -0.5f, 0.0f, 0.2f, 0.7f, 0.5f);
    // set6(2, -0.5f, -0.5f, 0.0f, 0.6f, 0.3f, 0.8f);
    // set6(3, -0.5f,  0.5f, 0.0f, 0.3f, 0.5f, 0.9f);
  }
};

C* cs = new C[cubes_count];

void InitCubes()
{
  float scale = 0.3f;
  for(int i = 0; i < cubes_count; ++i) {
    cs[i].p.x = i / 1.0f;
    cs[i].s.x = cs[i].s.y = cs[i].s.z = scale;
    if (i == 6) {
      cs[i].c_lfu = cs[i].c_lbu = cs[i].c_lbd = cs[i].c_lfd = {1.0f, 1.0f, 1.0f};
    }
    if (i == 5) {
      cs[i].c_lfu = cs[i].c_lbu = cs[i].c_lbd = cs[i].c_lfd =
        cs[i].c_rfu = cs[i].c_rbu = cs[i].c_rbd = cs[i].c_rfd =
        {1.0f, 0.0f, 0.0f};
    }
    if (i == 4) {
      cs[i].c_rfu = cs[i].c_rbu = cs[i].c_rbd = cs[i].c_rfd = {1.0f, 1.0f, 1.0f};
    }
    cs[i].WriteBuffer(i * 24);
  }

  initLineList();
}

  //  {  1.5f,  0.5f, 0.0f, 0.5f, 0.5f, 0.5f },
  //  {  1.5f, -0.5f, 0.0f, 0.2f, 0.7f, 0.5f },
  //  { 0.5f, -0.5f, 0.0f, 0.6f, 0.3f, 0.8f },
  //  { 0.5f,  0.5f, 0.0f, 0.3f, 0.5f, 0.9f },
  // };

// static const uint16_t s_cubeTriList[] =
//   {
//    0,1,3,
//    1,2,3
//   };

bgfx::VertexBufferHandle m_vbh;
bgfx::IndexBufferHandle m_ibh;
bgfx::ProgramHandle m_program;

int main (int argc, char* args[]) {

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
  InitCubes();
  m_vbh = bgfx::createVertexBuffer(
              // Static data can be passed with bgfx::makeRef
              bgfx::makeRef(s_cubeVertices, vertices_count * sizeof(PosColorVertex)),
              PosColorVertex::ms_layout
          );

  m_ibh = bgfx::createIndexBuffer(
              // Static data can be passed with bgfx::makeRef
              // bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
              bgfx::makeRef(s_cubeLineList, lines_count * sizeof(uint16_t))
          );

  bgfx::ShaderHandle vsh = loadShader("bin/v_simple.bin");
  bgfx::ShaderHandle fsh = loadShader("bin/f_simple.bin");

  m_program = bgfx::createProgram(vsh,fsh,  true);



  // Reset window
  bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);

  // Enable debug text.
  bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

  // Set view rectangle for 0th view
  bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));

  // Clear the view rect
  bgfx::setViewClear(0,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     0x443355FF, 1.0f, 0);


  // Set empty primitive on screen
  // bgfx::touch(0);

  float x = 0.0, y = 0.0, mx = 0.02, my = 0.01;

  // Poll for events and wait till user closes window
  bool quit = false;
  uint32_t last_time = SDL_GetTicks(), current_time = SDL_GetTicks(), dt;
  SDL_Event currentEvent;
  while(!quit) {
    while(SDL_PollEvent(&currentEvent)) {
      if(currentEvent.type == SDL_QUIT) {
        quit = true;
      }
    }

    dt = current_time - last_time;

    const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
    // const bx::Vec3 eye = { 0.0f, 0.0f, 7.0f };
    const bx::Vec3 eye = { -5.0f, 4.0f, 7.0f };

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
    if (abs(x) >= 3) {
      mx *= -1;
    }
    if (abs(y) >= 3) {
      my *= -1;
    }

    float mtx[16];
    bx::mtxSRT(mtx, 3, 3, 3, 0, 0, 0, x, y, 0);

    // Set model matrix for rendering.
    bgfx::setTransform(mtx);

    // Set vertex and index buffer.
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);

    // Set render states.
    bgfx::setState(0
        | BGFX_STATE_WRITE_R
        | BGFX_STATE_WRITE_G
        | BGFX_STATE_WRITE_B
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_WRITE_Z
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_CULL_CW
        | BGFX_STATE_MSAA
        | BGFX_STATE_PT_LINES
        // | BGFX_STATE_PT_LINESTRIP
        );

    // Submit primitive for rendering to view 0.
    bgfx::submit(0, m_program);

    bgfx::frame();

    last_time = current_time;
    current_time = SDL_GetTicks();
  }

  bgfx::shutdown();
  // Free up window
  SDL_DestroyWindow(window);
  // Shutdown SDL
  SDL_Quit();

  return 0;
}
