///
///   \file ocullScene.hpp
///
///

#ifndef OCULL_SCENE_HPP_
#define OCULL_SCENE_HPP_

#include <cstdlib>
#include <vector>

#include "rasterizer/framework/gpu/Buffer.hpp"
#include "ocullDefs.hpp"

#define OCULLSCENE_CACHE_FRIENDLY  0


namespace ocull {

struct Mesh
{
  public:
    struct Vertex
    {
      FW::Buffer buffer;
      size_t offset;
      size_t count;
      size_t stride;//
    } vertex;

    struct Index
    {
      FW::Buffer buffer;
      size_t offset;
      size_t count;
    } index;
  
  
  public:
    // GL mesh
    Mesh( unsigned int vbo, size_t vOffset, size_t vCount, size_t vStride,
          unsigned int ibo, size_t iOffset, size_t iCount)
    {
      vertex.buffer.wrapGL(vbo);
      vertex.offset = vOffset;
      vertex.count  = vCount;
      vertex.stride = vStride;
      
      vertex.buffer.wrapGL(ibo);
      vertex.offset = iOffset;
      vertex.count = iCount;
    }
    
    // CPU mesh
    Mesh( float *vertices, size_t vCount,
          unsigned int *indices,  size_t iCount)
    {
      vertex.buffer.wrapCPU( vertices, vCount * 3u * sizeof(float));
      vertex.offset = 0u;
      vertex.count = vCount;
      vertex.stride = 0u;   
      
      index.buffer.wrapCPU( indices, iCount * sizeof(unsigned int));
      index.offset = 0u;
      index.count = iCount; 
    }
    
    inline unsigned int getTriangleCount() const {return index.count / 3u;}
};


struct Scene
{
  public:
    /// [ Geometric datas ]
#   if !(OCULLSCENE_CACHE_FRIENDLY)
    std::vector<ocull::Mesh> meshes;
#   else
    // (supposed) more cache efficient
    std::vector<FW::Buffer> vertices;
    std::vector<FW::Buffer> indices;
    std::vector<FW::Vec3i>  vertexParams;
    std::vector<FW::Vec2i>  indexParams;
#   endif

    /// [ per Instance attributes ]
    std::vector<unsigned int> meshesId;
    std::vector<ocull::Matrix4x4> worldMatrices;
  
  
  public:
    unsigned int insertMesh(const Mesh& mesh);
    
    void insertModel( const unsigned int meshId, const Matrix4x4& modelMatrix);
    
    void reset();
};

} // namespace ocull

#endif //OCULL_SCENE_HPP_
