#include "porter.hpp"

void Porter::import(const char* filename)
{
  sprintf(filename_str, "assets/%s", filename);
  const aiScene* scene = aiImportFile(filename_str,
    aiProcess_CalcTangentSpace       |
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);

  if (!scene) {
    printf(aiGetErrorString());
    return;
  }

  // Now we can access the file's contents

  aiReleaseImport(scene);
}
