///
///   \file ocullContext.hpp
///
///   \note Using static for Creation / Destruction may not be a clever idea.
///         I don't want the object to be statically initialized, because it
///         could bypass some other lib initialization.. ok I can let this
///         to the user, but it's a simple trick to 'solve' the problem.
///

#ifndef OCULL_CONTEXT_HPP_
#define OCULL_CONTEXT_HPP_

#include <string>

#include "rasterizer/framework/base/Defs.hpp"
#include "rasterizer/CudaRaster.hpp"
#include "ocullDefs.hpp"



namespace ocull {

class Query;
struct Scene;
struct Mesh;

class Context
{  
  private:
    FW::CudaModule *m_cudaModule;
    FW::CudaRaster  m_rasterizer;
    
    // Vertex Shader
    CUfunction m_vsKernel;
    FW::Buffer m_outVertices;     // VS output vertices (in clip-space)
    
    // Currently bound query
    ocull::Query *m_pQuery;
    
    // [DEBUG only] (to remove later)
    FW::CudaSurface *m_colorBuffer;
    
    
  public:
    static Context* Create(const std::string &pipeCubinFile);
    static void Release(Context* context);
    
  
  public:
    void begin(Query *pQuery);
    void end();
    
    // MUST be called between begin() / end()
    void uploadScene(ocull::Scene *pScene);
    void uploadMesh(ocull::Mesh *pMesh, const ocull::Matrix4x4 &modelMatrix);
    
    // [DEBUG only] (to remove later)
    unsigned int getColorTexture();
  
  
  private:
    Context();
    ~Context();
    
    // true between begin() / end(), false otherwise
    inline bool isQueryBounded() { return m_pQuery != NULL; }
  
    
  // forbidden methods
  private:
    DISALLOW_COPY_AND_ASSIGN(Context);
};

} //namespace ocull


#endif //OCULL_CONTEXT_HPP_
