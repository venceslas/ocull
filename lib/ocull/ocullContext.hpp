///
///   
///     ~~ not great, rethink it ~~
///

#ifndef OCULL_CONTEXT_HPP_
#define OCULL_CONTEXT_HPP_

#include "rasterizer/CudaRaster.hpp"
#include "rasterizer/framework/base/Defs.hpp"
#include "rasterizer/framework/gpu/CudaCompiler.hpp"

#include "ocullDefs.hpp"


namespace ocull {

struct Mesh;
struct Query;

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
  
    //
    GLuint DEBUG_getColorTex() {return (m_colorBuffer)?m_colorBuffer->getGLTexture():0u;}//
    //
  
  private:
    /// set the depthBuffer surface used by the rasterizer and cleared it.
    void setRasterizerParams(FW::CudaSurface *depthBuffer);
    
    /// Transform the mesh data to clip space then send it through the 
    /// rasterizer pipeline.
    void uploadMesh(ocull::Mesh &mesh, const ocull::Matrix4x4 &worldMatrix);
};

} //namespace ocull

#endif //OCULL_CONTEXT_HPP_
