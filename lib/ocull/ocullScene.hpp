///
///
///
///

#ifndef OCULL_SCENE_HPP_
#define OCULL_SCENE_HPP_

#include <vector>

#include "rasterizer/framework/gpu/Buffer.hpp"
#include "ocullDefs.hpp"


namespace ocull {

// ~
struct Mesh
{
  struct Vertex
  {
    FW::Buffer buffer;
    size_t offset;
    size_t size;
    size_t stride;
  } vertex;

  struct Index
  {
    FW::Buffer buffer;
    size_t offset;
    size_t size;
  } index;
  
  
  //
  
  Mesh( GLuint vbo, size_t vertOffset, size_t vertexCount, size_t stride,
        GLuint ibo, size_t triOffset, size_t indexCount)
  {
    vertex.buffer.wrapGL(vbo);
    vertex.offset = vertOffset;
    vertex.size = vertexCount;
    vertex.stride = stride;
    
    index.buffer.wrapGL(ibo);
    index.offset = triOffset;
    index.size = indexCount;//
  }
  
  Mesh( float *vertices, size_t vertexCount,
        unsigned int *indices, size_t indexCount)
  {
  }
  
  unsigned int getTriangleCount() const { return index.size / 3u; }
};


//typedef meshIdx_t unsigned int;

//
struct Scene 
{
  /// [ Geometric datas ]
  std::vector<ocull::Mesh> meshes;

  // TODO change for a cache efficient structure :
  //std::vector<FW::Buffer> vertices;
  //std::vector<FW::Buffer> indices;
  //std::vector<FW::Vec3ui> vertexParams;
  //std::vector<FW::Vec2ui> indexParams;
  
  /// [ per Instance attributes ]
  std::vector<unsigned int> meshesId;
  std::vector<ocull::Matrix4x4> worldMatrices;
  
  //--
  
  unsigned int insertMesh( const ocull::Mesh &mesh );//
  void insertModel( unsigned int meshId, Matrix4x4 worldMatrix);//      
  void reset();//
  
};

} // namespace ocull

#endif //OCULL_SCENE_HPP_
