#include "porter.hpp"

void processMesh(aiMesh *mesh, const aiScene *scene, BufferObject& bo)
{
  for(int i = 0; i < mesh->mNumVertices; i++) {
    bo.vertices[i].x = mesh->mVertices[i].x;
    bo.vertices[i].y = mesh->mVertices[i].y;
    bo.vertices[i].z = mesh->mVertices[i].z;
    bo.vertices[i].r = 0.5f;
    bo.vertices[i].g = 0.3f;
    bo.vertices[i].b = 0.9f;
    bo.vertices[i].normal_x = mesh->mNormals[i].x;
    bo.vertices[i].normal_y = mesh->mNormals[i].y;
    bo.vertices[i].normal_z = mesh->mNormals[i].z;
  }

  int n = 0;
  for(int j = 0; j < mesh->mNumFaces; j++) {
    for(int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
      bo.indices[n] = mesh->mFaces[j].mIndices[k];
      n += 1;
    }
  }

  // if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
  // {
  //   glm::vec2 vec;
  //   vec.x = mesh->mTextureCoords[0][i].x;
  //   vec.y = mesh->mTextureCoords[0][i].y;
  //   vertex.TexCoords = vec;
  // }
}


void processNode(aiNode* node, const aiScene* scene, BufferObject& bo)
{
  for(int i = 0; i < node->mNumMeshes; ++i) {
    processMesh(scene->mMeshes[node->mMeshes[i]], scene, bo);
  }

  for(int i = 0; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene, bo);
  }
}


void countMeshVerticesAndIndices(aiMesh *mesh, const aiScene *scene, BufferObject& bo)
{
  bo.vertices_count += mesh->mNumVertices;
  for(int j = 0; j < mesh->mNumFaces; j++) {
    bo.indices_count += mesh->mFaces[j].mNumIndices;
  }
}

void countVerticesAndIndices(aiNode* node, const aiScene* scene, BufferObject& bo)
{
  bo.vertices_count = 0;
  bo.indices_count = 0;

  for(int i = 0; i < node->mNumMeshes; ++i) {
    countMeshVerticesAndIndices(scene->mMeshes[node->mMeshes[i]], scene, bo);
  }

  for(int i = 0; i < node->mNumChildren; ++i) {
    countVerticesAndIndices(node->mChildren[i], scene, bo);
  }
}


void Porter::import(const char* filename, BufferObject& bo)
{
  sprintf(filename_str, "assets/%s", filename);
  printf("Importing %s\n", filename_str);
  const aiScene* scene = aiImportFile(filename_str,
    aiProcess_CalcTangentSpace       |
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);

  if (!scene) {
    printf("Errors: %s\n", aiGetErrorString());
    return;
  }

  countVerticesAndIndices(scene->mRootNode, scene, bo);
  bo.vertices = new PosColorVertex[bo.vertices_count];
  bo.indices = new uint16_t[bo.indices_count];

  processNode(scene->mRootNode, scene, bo);

  aiReleaseImport(scene);
}
