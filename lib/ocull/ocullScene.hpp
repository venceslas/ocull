///
///   \file ocullScene.hpp
///   

#ifndef OCULL_SCENE_HPP_
#define OCULL_SCENE_HPP_

#include <vector>

#include "ocullDefs.hpp"
#include "rasterizer/framework/gpu/Buffer.hpp"


namespace Ocull {

/// 
/// Holds geometrics attribute for a 3D mesh
/// 
class Model
{
  friend class Scene;
  
  
  private:
    /// OpenGL buffer (for debugging visualization)
    //VertexBuffer m_vertexBuffer;
    
    /// CudaRaster buffer (for visibility tests)
    FW::Buffer m_inVertices;          // vs input vertices
    FW::Buffer m_outVertices;         // vs output vertices (transformed) [share it ?]
    FW::Buffer m_indices;
    
    int m_numVertices;
    int m_numTriangles;
  
  
  public:
    const Ocull::Vector3* getVertices() const;
    const int getVertexCount() const { return m_numVertices; }
        
    const Ocull::Vector3i* getTriangles() const;
    const int getTriangleCount() const { return m_numTriangles; }
    
  
  private:
    Model(const float *vertices, 
          const unsigned int *indices,
          int vertexCount, 
          int triangleCount, 
          int vertexStride);
        
    DISALLOW_COPY_AND_ASSIGN(Model);
};

///
/// Instance of a Model
///
class Object
{
  friend class Scene;
  
  
  private:
    const Ocull::Model *m_pModel;
    Ocull::Matrix4x4 m_worldMatrix;
    unsigned int m_id;
    unsigned int m_flags;
    
    
  public:
    const Ocull::Model* getModel() const { return m_pModel; }
    void getMatrix(Ocull::Matrix4x4 &m) const { m = m_worldMatrix; }
    
  
  private:    
    Object(const Ocull::Model *model, const Ocull::Matrix4x4 &m, unsigned int id, 
           unsigned int flags);
    
    DISALLOW_COPY_AND_ASSIGN(Object);
};  


///
/// Internal representation of a 3D scene
///
class Scene
{ 
  private:
    std::vector<Model*> m_Models;
    std::vector<Object*> m_Objects;
    
  
  public:
    //static Scene* Create(const char *filename=NULL);      
  
  public:
    Scene() {}
    ~Scene() {release();}
    
    void release();    
    
    // === Model ===
    const Model* insertModel(const float *vertices,
                             const unsigned int *indices,
                             int vertexCount,
                             int triangleCount,
                             int vertexStride=0);
    
    unsigned int getModelCount(void) const;
    
    const Model* getModel(unsigned int index) const;
    
    
    // === Object ===
    const Object* insertObject(const Model *model,
                               const Ocull::Matrix4x4 &m,
                               unsigned int id,
                               unsigned int flags=0u);
    
    unsigned int getObjectCount(void) const;
    
    const Object* getObject(unsigned int index) const;
    
  
  private:
    DISALLOW_COPY_AND_ASSIGN(Scene);
};

}

#endif //OCULL_SCENE_HPP_
