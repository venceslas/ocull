///
///
///
///

#ifndef OCULL_QUERY_HPP_
#define OCULL_QUERY_HPP_

#include "ocullCamera.hpp"
#include "ocullContext.hpp"
#include "ocullDefs.hpp"

namespace FW {
class CudaSurface;
}

namespace ocull {

class Scene;
class Mesh;

class Query
{
  private:
    Context &m_context;
    FW::CudaSurface *m_DepthBuffer;
    
    struct Stats 
    {    
      std::vector<unsigned int> samplesPassed;
      unsigned int numObjects;
      unsigned int numObjectsPassed;
      unsigned int numTriangles;
      unsigned int numTrianglesPassed;      
      float queryTime;
    } m_stats;
    
    // States
    const ocull::Camera *m_pCamera;
    bool m_bHasBegun;
    

  public:
    Query(Context &context)
        : m_context(context),
          m_DepthBuffer(NULL),
          m_pCamera(NULL),
          m_bHasBegun(false)
    {
      //context.addQuery(this);
      resetStats();
    }
    
    ~Query();
    

    // Resize the depth buffer
    void resize(unsigned int w, unsigned int h);
    
    /// +++ QUERY +++
    void begin(const ocull::Camera &camera, 
               const ocull::DepthBuffer *pDepthBuffer=NULL);
    void end();
    
    void uploadScene(const ocull::Scene &scene);
    void uploadMesh(const ocull::Mesh &mesh, const ocull::Matrix4x4 &worldMatrix);
    
    /// +++ GETTERS +++
    void getSamplesPassed(unsigned int *samplesPassed);
    void getSamplesPassed(unsigned int *samplesPassed, size_t count);
    
    void getDepthBuffer(ocull::DepthBuffer *depthBuffer);

    //TODO
    //void getQueryTime();
    //void getNumObjects();
    //void getNumPassed();
    //void getNumTriangles();
    //void getNumTrianglesPassed();

  private:
    void clearDepth(const ocull::DepthBuffer *pDepthBuffer);
    void resetStats();
};

} // namespace ocull

#endif //OCULL_QUERY_HPP_
