#ifndef OCULL_CONTEXT_HPP_
#define OCULL_CONTEXT_HPP_

namespace ocull {

class Context
{
  friend ocull::Query;
  
  private:
    FW::CudaCompiler m_cudaCompiler;
    FW::CudaModule *m_CudaModule;
    CUfunction m_vertexShaderKernel;
    
    FW::CudaRaster m_cudaRaster;
    
    // Vertex shaders output (transformed) vertices, in clip-space
    FW::Buffer m_csVertices;
  
  public:
    Context() : m_CudaModule(NULL) {}
    ~Context();
    
    // for each query, call resize
    //void resize(uint w, uint h);
    
    /// Load the preprocessed Pipeline, or compiles it if needed
    /// Must be call after the window's context & CUDA has been initialized.
    void init();
    
    /// ~
    /// Transform mesh data from object to clip space to be send through the
    /// pipeline.
    /// New vertices positions are stored into m_csVertices, which is shared
    /// by all queries using the same context.
    void objectToClipSpace( const ocull::Mesh &mesh, 
                            const ocull::Matrix4x4 mvp);
};

} //namespace ocull

#endif //OCULL_CONTEXT_HPP_
