#include "models.hpp"

void Models::processMesh(aiMesh *mesh, const aiScene *scene, int& vertices_offset, int& indices_offset)
{
  for(int i = 0; i < mesh->mNumVertices; i++) {
    vertices[vertices_offset + i].x = mesh->mVertices[i].x;
    vertices[vertices_offset + i].y = mesh->mVertices[i].y;
    vertices[vertices_offset + i].z = mesh->mVertices[i].z;
    vertices[vertices_offset + i].r = 0.5f;
    vertices[vertices_offset + i].g = 0.3f;
    vertices[vertices_offset + i].b = 0.9f;
    vertices[vertices_offset + i].normal_x = mesh->mNormals[i].x;
    vertices[vertices_offset + i].normal_y = mesh->mNormals[i].y;
    vertices[vertices_offset + i].normal_z = mesh->mNormals[i].z;
  }

  vertices_offset += mesh->mNumVertices;

  int n = 0;
  for(int j = 0; j < mesh->mNumFaces; j++) {
    for(int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
      indices[indices_offset + n] = mesh->mFaces[j].mIndices[k];
      n += 1;
    }
  }

  indices_offset += n;

  // if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
  // {
  //   glm::vec2 vec;
  //   vec.x = mesh->mTextureCoords[0][i].x;
  //   vec.y = mesh->mTextureCoords[0][i].y;
  //   vertex.TexCoords = vec;
  // }
}


int Models::processNode(aiNode* node, const aiScene* scene, int& vertices_offset, int& indices_offset)
{
  for(int i = 0; i < node->mNumMeshes; ++i) {
    processMesh(scene->mMeshes[node->mMeshes[i]], scene, vertices_offset, indices_offset);
  }

  for(int i = 0; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene, vertices_offset, indices_offset);
  }
}


void Models::init()
{
  vertices = new PosColorVertex[vertices_count];
  indices = new uint16_t[indices_count];
  vertices_offsets[0] = 0;
  indices_offsets[0] = 0;

  import("test.obj", 0);
}


void Models::import(const char* filename, const int nth)
{
  sprintf(filename_str, "assets/%s", filename);
  const aiScene* scene = aiImportFile(filename_str,
    aiProcess_CalcTangentSpace       |
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);

  if (!scene) {
    printf("Errors: %s\n", aiGetErrorString());
    return;
  }

  int acc_next_vertices_offset = 0;
  int acc_next_indices_offset = 0;

  processNode(scene->mRootNode, scene, acc_next_vertices_offset, acc_next_indices_offset);

  vertices_offsets[nth + 1] = acc_next_vertices_offset;
  indices_offsets[nth + 1] = acc_next_indices_offset;

  aiReleaseImport(scene);
}


int Models::nth_model_vertices_count(int nth) const
{
  return vertices_offsets[nth + 1] - vertices_offsets[nth];
}


int Models::nth_model_indices_count(int nth) const
{
  return indices_offsets[nth + 1] - indices_offsets[nth];
}
