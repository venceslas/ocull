///
///   ocullScene.hpp
///   
///   

#include "ocullScene.hpp"

#include <cassert>
#include <vector>

// cudaraster's kernel shader header
#include "rasterizer/shader/PassThrough.hpp"


namespace Ocull {

// ----------------------
// Model
// ----------------------

Model::Model(const float *vertices, const unsigned int *indices,
             int vertexCount, int triangleCount, int vertexStride)
{
  assert( NULL != vertices );
  assert( NULL != indices );
  
  
  m_numVertices = vertexCount;
  m_numTriangles = triangleCount;  
    
  /// Allocate buffers
  m_inVertices.resizeDiscard( m_numVertices  * sizeof(FW::InputVertex) );
  m_outVertices.resizeDiscard( m_numVertices * sizeof(FW::ShadedVertex_passthrough) );  
  m_indices.resizeDiscard( m_numTriangles * sizeof(FW::Vec3i) ); 
      
  /// Copy vertex positions
  FW::InputVertex* inputVertexPtr = (FW::InputVertex*)m_inVertices.getMutablePtr();
  
  for (int i=0; i < m_numVertices; ++i)
  {
    FW::Vec3f &vertex = inputVertexPtr[i].modelPos;    
    vertex = FW::Vec3f::fromPtr( &vertices[3*i] + vertexStride );//
  }
    
  /// Copy vertex indices
  FW::Vec3i* vertexIndexPtr = (FW::Vec3i*)m_indices.getMutablePtr();    
  
  for (int i=0; i < m_numTriangles; ++i)
  {
    //vertexIndexPtr[i] = FW::Vec3i::fromPtr( &indices[3*i] );
    
    FW::Vec3i &triIdx = vertexIndexPtr[i];
    
    // conversion from unsigned to signed integer xxx
    triIdx.x = static_cast<FW::S32>( indices[3*i + 0] );
    triIdx.y = static_cast<FW::S32>( indices[3*i + 1] );
    triIdx.z = static_cast<FW::S32>( indices[3*i + 2] );
  }
}


// ----------------------
// Object
// ----------------------

Object::Object(const Ocull::Model *model, const Ocull::Matrix4x4 &m,
               unsigned int id, unsigned int flags)
{
  assert( NULL != model );
  
  m_pModel = model;
  m_worldMatrix = m;
  m_id = id;
  m_flags = flags;
}


// ----------------------
// Scene
// ----------------------          

void Scene::release()
{  
  if (!m_Models.empty())
  {
    std::vector<Model*>::iterator it;
    
    for (it=m_Models.begin(); it != m_Models.end(); ++it)
    {
      delete *it;
    }
    m_Models.clear();
  }
  
  if (!m_Objects.empty())
  {
    std::vector<Object*>::iterator it;
    
    for (it=m_Objects.begin(); it != m_Objects.end(); ++it)
    {
      delete *it;
    }
    m_Objects.clear();
  }
}


const Model* Scene::insertModel(const float *vertices, 
                                const unsigned int *indices,
                                int vertexCount, 
                                int triangleCount, 
                                int vertexStride)
{
  Model* model = new Model(vertices, indices, vertexCount, triangleCount, 
                           vertexStride);
  
  m_Models.push_back(model);
  
  return model;
}


unsigned int Scene::getModelCount(void) const 
{ 
  return (m_Models.empty()) ? 0u : m_Models.size();
}


const Model* Scene::getModel(unsigned int index) const 
{ 
  return (index < getModelCount()) ? m_Models[index] : NULL;
}


const Object* Scene::insertObject(const Ocull::Model *model, 
                                  const Ocull::Matrix4x4 &m,
                                  unsigned int id, 
                                  unsigned int flags)
{
  Object* object = new Object(model, m, id, flags);
  
  m_Objects.push_back(object);
    
  return object;
}


unsigned int Scene::getObjectCount(void) const
{
  return (m_Objects.empty()) ? 0u : m_Objects.size();
}


const Object* Scene::getObject(unsigned int index) const
{
  return (index < getObjectCount()) ? m_Objects[index] : NULL;
}

} // namespace Ocull
