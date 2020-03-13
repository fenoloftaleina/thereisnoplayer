#include "models.hpp"

void Models::processMesh(aiMesh *mesh, const aiScene *scene, int& vertices_offset, int& indices_offset)
{
  // printf("Mesh with %d vertices and %d faces.\n", mesh->mNumVertices, mesh->mNumFaces);
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

  int n = 0;
  for(int j = 0; j < mesh->mNumFaces; j++) {
    for(int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
      indices[indices_offset + n] = mesh->mFaces[j].mIndices[k];
      n += 1;
    }
  }

  vertices_offset += mesh->mNumVertices;
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
  // printf("Node with %d meshes and %d children.\n", node->mNumMeshes, node->mNumChildren);
  for(int i = 0; i < node->mNumMeshes; ++i) {
    processMesh(scene->mMeshes[node->mMeshes[i]], scene, vertices_offset, indices_offset);
  }

  for(int i = 0; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene, vertices_offset, indices_offset);
  }
}


void Models::init()
{
  vertices = new AnimatedPosColorTexVertex[vertices_count];
  indices = new uint16_t[indices_count];
  vertices_offsets[0] = 0;
  indices_offsets[0] = 0;


  // TODO:
  // Move import-ing and set-ing these below to buffer objects that now each
  // have their own models.


  import("test.obj", 0);
  import("test-keyframe2.obj", 1);
  import("untitled.obj", 2);
  import("cube.obj", 3);

  std::vector<bx::Vec3> vertices;
  std::vector<bx::Vec3> normals;
  std::vector<bx::Vec3> uvs;
  std::vector<int> indices;

  vertices.resize(4);
  normals.resize(4);
  uvs.resize(4);
  indices.resize(6);

  vertices[0] = bx::Vec3( 1.0f, -1.0f, -1.0f);
  vertices[1] = bx::Vec3( 1.0f, -1.0f,  1.0f);
  vertices[2] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  vertices[3] = bx::Vec3(-1.0f, -1.0f,  1.0f);

  normals[0] = normals[1] = normals[2] = normals[3] = bx::Vec3(0.0f, 1.0f, 0.0f);

  // uvs[0] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  // uvs[1] = bx::Vec3( 1.0f, -1.0f,  1.0f);
  // uvs[2] = bx::Vec3(-1.0f, -1.0f, -1.0f);
  // uvs[3] = bx::Vec3(-1.0f, -1.0f,  1.0f);

  uvs[0] = bx::Vec3(textures->mappings[0].x2, textures->mappings[0].y1, 0.0f);
  uvs[1] = bx::Vec3(textures->mappings[0].x2, textures->mappings[0].y2, 0.0f);
  uvs[2] = bx::Vec3(textures->mappings[0].x1, textures->mappings[0].y1, 0.0f);
  uvs[3] = bx::Vec3(textures->mappings[0].x1, textures->mappings[0].y2, 0.0f);

  // uvs[0] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  // uvs[1] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  // uvs[2] = bx::Vec3(-1.0f, -1.0f, 0.0f);
  // uvs[3] = bx::Vec3(-1.0f, -1.0f, 0.0f);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 3;
  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  set(vertices, normals, uvs, indices, 4);

  // import("cube.obj", 0);
  //
  // for (int i = 0; i < vertices_offsets[1]; ++i) {
  //   printf("v %f %f %f\n", vertices[i].normal_x, vertices[i].normal_y, vertices[i].normal_z);
  // }
  // printf("\n");
  // // for (int i = 0; i < indices_offsets[1]; ++i) {
  // //   printf("i %d\n", indices[i]);
  // // }
  // // printf("\n");
  //
  // exit(0);
}


void Models::import(const char* filename, const int nth)
{
  sprintf(filename_str, "assets/%s", filename);
  const aiScene* scene = aiImportFile(filename_str,
      // aiProcess_Triangulate | aiProcess_FlipUVs
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType
    );

  if (!scene) {
    printf("Errors: %s\n", aiGetErrorString());
    return;
  }

  int acc_next_vertices_offset = vertices_offsets[nth];
  int acc_next_indices_offset = indices_offsets[nth];

  processNode(scene->mRootNode, scene, acc_next_vertices_offset, acc_next_indices_offset);

  vertices_offsets[nth + 1] = acc_next_vertices_offset;
  indices_offsets[nth + 1] = acc_next_indices_offset;

  aiReleaseImport(scene);
}


void Models::set
(const std::vector<bx::Vec3>& model_vertices,
 const std::vector<bx::Vec3>& model_normals,
 const std::vector<bx::Vec3>& model_uvs,
 const std::vector<int>& model_indices,
 const int nth)
{
  int vertices_offset = vertices_offsets[nth];
  int indices_offset = indices_offsets[nth];

  for(int i = 0; i < model_vertices.size(); i++) {
    vertices[vertices_offset + i].x = model_vertices[i].x;
    vertices[vertices_offset + i].y = model_vertices[i].y;
    vertices[vertices_offset + i].z = model_vertices[i].z;
    vertices[vertices_offset + i].r = 0.5f;
    vertices[vertices_offset + i].g = 0.3f;
    vertices[vertices_offset + i].b = 0.9f;
    vertices[vertices_offset + i].normal_x = model_normals[i].x;
    vertices[vertices_offset + i].normal_y = model_normals[i].y;
    vertices[vertices_offset + i].normal_z = model_normals[i].z;
    vertices[vertices_offset + i].texcoord_x1 = model_uvs[i].x;
    vertices[vertices_offset + i].texcoord_y1 = model_uvs[i].y;
  }

  for(int i = 0; i < model_indices.size(); i++) {
    indices[indices_offset + i] = model_indices[i];
  }

  vertices_offsets[nth + 1] = vertices_offset + model_vertices.size();
  indices_offsets[nth + 1] = indices_offset + model_indices.size();
}


int Models::nth_model_vertices_count(int nth) const
{
  return vertices_offsets[nth + 1] - vertices_offsets[nth];
}


int Models::nth_model_indices_count(int nth) const
{
  return indices_offsets[nth + 1] - indices_offsets[nth];
}
