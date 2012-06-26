///
///   
///
///

#ifndef OCULL_CONTEXT_HPP_
#define OCULL_CONTEXT_HPP_

#include "rasterizer/CudaRaster.hpp"
#include "rasterizer/framework/base/Defs.hpp"
#include "rasterizer/framework/gpu/CudaCompiler.hpp"

#include "ocullDefs.hpp"


namespace ocull {

class Mesh;
class Query;

class Context
{  
  friend class Query;
  
  private:
    FW::CudaCompiler m_cudaCompiler;
    FW::CudaModule *m_CudaModule;
    CUfunction m_vertexShaderKernel;
    
    FW::CudaRaster m_cudaRaster;
    
    // Vertex shaders output (transformed) vertices, in clip-space
    FW::Buffer m_csVertices;
    
    // TODO
    // A 'fake' color buffer, used to not have to remove the framebuffer from the
    // rasterizer yet.
    FW::CudaSurface *m_colorBuffer;
  

  public:
    Context()
      : m_CudaModule(NULL),
        m_colorBuffer(NULL)
    {}
    ~Context();
    
    // (calls resize for each query ?)
    //void resize(uint w, uint h);
    
    /// Load the preprocessed Pipeline, or compiles it if needed.
    /// Must be call after the window's context & CUDA has been initialized.
    void init();
    
    /// ~
    /// Transform mesh data from object to clip space to be send through the
    /// pipeline.
    /// New vertices positions are stored into m_csVertices, which is shared
    /// by all queries using the same context.
    void objectToClipSpace( const ocull::Mesh &mesh, 
                            const ocull::Matrix4x4 &mvp);

  private:
    void setRasterizerParams(FW::CudaSurface *depthBuffer);
};

} //namespace ocull

#endif //OCULL_CONTEXT_HPP_
