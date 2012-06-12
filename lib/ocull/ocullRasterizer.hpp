///
///   /file ocullRasterizer.hpp
///

#ifndef OCULL_RASTERIZER_HPP_
#define OCULL_RASTERIZER_HPP_

#include "rasterizer/CudaRaster.hpp"
#include "rasterizer/framework/base/Defs.hpp"
#include "rasterizer/framework/gpu/CudaCompiler.hpp"

#include "ocullDefs.hpp"


namespace Ocull {

class Camera;
class Object;

///
/// Interface to use the CudaRasterizer
///
class Rasterizer
{
  private:
    FW::CudaCompiler m_cudaCompiler;
    FW::CudaRaster m_cudaRaster;
    
    bool m_pipeDirty;
    FW::CudaSurface* m_depthBuffer;
    FW::CudaModule* m_cudaModule;
    CUfunction m_vertexShaderKernel;
    
  
  public:
    Rasterizer()
        : m_pipeDirty(true),
          m_depthBuffer(NULL),
          m_cudaModule(NULL),
          m_vertexShaderKernel(NULL)
    {}
    
    ~Rasterizer();
    
    void init();
    void render( const Ocull::Camera& camera, const Ocull::Object& object);
    
    
  private:
    void initPipe();

    DISALLOW_COPY_AND_ASSIGN(Rasterizer);
}; 

} // namespace Ocull

#endif // OCULL_RASTERIZER_HPP_
