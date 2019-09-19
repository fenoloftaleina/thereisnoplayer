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
  float x;
  float y;
  float z;
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

static const int cubes_count = 1;
static const int faces_per_cube_count = 6;
static const int vertices_per_face_count = 4;
static const int vertices_per_cube_count = faces_per_cube_count * vertices_per_face_count;

static const int vertices_count = vertices_per_cube_count * cubes_count;
static PosColorVertex* vertices = new PosColorVertex[vertices_count];

static const int indices_per_face_count = 6;

static const int indices_count = indices_per_face_count * faces_per_cube_count * cubes_count;
static uint16_t* indices = new uint16_t[indices_count];

static const bx::Vec3 constexpr pos_vertices[vertices_per_cube_count] = {
  // CCW, indices: 0, 1, 3, 1, 2, 3, means: 'u' shapes starting on the right

  // front
  {  1.0f,  1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},

  // right
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f,  1.0f, -1.0f,},

  // back
  { -1.0f,  1.0f,  1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  {  1.0f, -1.0f,  1.0f,},
  {  1.0f,  1.0f,  1.0f,},

  // left
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //top
  {  1.0f,  1.0f,  1.0f,},
  {  1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f, -1.0f,},
  { -1.0f,  1.0f,  1.0f,},

  //bottom
  { -1.0f, -1.0f,  1.0f,},
  { -1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f, -1.0f,},
  {  1.0f, -1.0f,  1.0f,},
};

// static const bx::Vec3 constexpr col_vertices[vertices_per_cube_count] = {
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
//   {0.5f, 0.5f, 0.5f},
// };

struct Cube
{
  // CubePosition pos;
  // CubeColor col;

  bx::Vec3 pos {0.0f, 0.0f, 0.0f};
  bx::Vec3 col {0.7f, 0.7f, 0.7f};

  // vertices[0] = {  1.0f,  1.0f, 1.0f, 0.5f, 0.5f, 0.5f };
  // vertices[1] = {  1.0f, -1.0f, 1.0f, 0.2f, 0.7f, 0.5f };
  // vertices[2] = { -1.0f, -1.0f, 1.0f, 0.6f, 0.3f, 0.8f };
  // vertices[3] = { -1.0f,  1.0f, 1.0f, 0.3f, 0.5f, 0.9f };

  void writeVertex(int offset, int i)
  {
    bx::Vec3 end_pos = bx::add(pos_vertices[i], pos);

    printf("%d %d - %f %f %f\n", offset, i, end_pos.x, end_pos.y, end_pos.z);

    vertices[offset + i].x = end_pos.x;
    vertices[offset + i].y = end_pos.y;
    vertices[offset + i].z = end_pos.z;
    vertices[offset + i].r = col.x;
    vertices[offset + i].g = col.y;
    vertices[offset + i].b = col.z;
  }

  void writeBuffer(int offset)
  {
    for (int i = 0; i < vertices_per_cube_count; ++i) {
      writeVertex(offset, i);
    }
  }
};


void initVertices()
{
  // vertices[0] = {  1.0f,  1.0f, 1.0f, 0.5f, 0.5f, 0.5f };
  // vertices[1] = {  1.0f, -1.0f, 1.0f, 0.2f, 0.7f, 0.5f };
  // vertices[2] = { -1.0f, -1.0f, 1.0f, 0.6f, 0.3f, 0.8f };
  // vertices[3] = { -1.0f,  1.0f, 1.0f, 0.3f, 0.5f, 0.9f };

  Cube c;

  c.writeBuffer(0);
}

void initIndices()
{
  for (int i = 0; i < indices_count / 6; ++i) {
    printf("indices %d\n", i);
    indices[i * 6 + 0] = i * 4 + 0;
    indices[i * 6 + 1] = i * 4 + 1;
    indices[i * 6 + 2] = i * 4 + 3;
    indices[i * 6 + 3] = i * 4 + 1;
    indices[i * 6 + 4] = i * 4 + 2;
    indices[i * 6 + 5] = i * 4 + 3;
  }
}

void initShit()
{
  initVertices();
  initIndices();
}

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
  initShit();

  m_vbh = bgfx::createVertexBuffer(
              // Static data can be passed with bgfx::makeRef
              bgfx::makeRef(vertices, vertices_count * sizeof(vertices[0])),
              PosColorVertex::ms_layout
          );

  m_ibh = bgfx::createIndexBuffer(
              // Static data can be passed with bgfx::makeRef
              bgfx::makeRef(indices, indices_count * sizeof(indices[0]))
              // bgfx::makeRef(indices, 6 * sizeof(indices[0]))
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
    // const bx::Vec3 eye = { -5.0f, 2.0f, -10.0f };
    // const bx::Vec3 eye = { 5.0f, 2.0f, -10.0f };
    const bx::Vec3 eye = {  10.0f, -10.0f, -13.0f };

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
        | BGFX_STATE_CULL_CCW
        | BGFX_STATE_MSAA
        // | BGFX_STATE_PT_LINES
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
