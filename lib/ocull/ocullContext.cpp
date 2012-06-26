///
///
///
///

#include "ocullContext.hpp"
#include <cstdio>

#include "rasterizer/shader/PassThrough.hpp" // kernel shader


namespace ocull {

Context::~Context()
{
  if (m_CudaModule != NULL)
  {
    delete m_CudaModule;
    m_CudaModule = NULL;
  }
}

void Context::init()
{
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );

  fprintf( stderr, "%s : check paths validity !! [TODO]\n", __FUNCTION__ );
  
  
  /// Compiles the pipeline shaders
  
  m_cudaCompiler.include( "lib/ocull/rasterizer" );
  m_cudaCompiler.include( "lib/ocull/rasterizer/framework" );  
  m_cudaCompiler.setSourceFile( "lib/ocull/rasterizer/shader/PassThrough.cu" );  
  
  // Compile CUDA code
  FW::U32 renderModeFlags = 0u;
  renderModeFlags |= FW::RenderModeFlag_EnableDepth;
  renderModeFlags |= FW::RenderModeFlag_EnableLerp;
  renderModeFlags |= FW::RenderModeFlag_EnableQuads;
  
  m_cudaCompiler.clearDefines();
  m_cudaCompiler.define( "SAMPLES_LOG2", 0);//
  m_cudaCompiler.define( "RENDER_MODE_FLAGS", renderModeFlags);
  m_cudaCompiler.define( "BLEND_SHADER", "BlendReplace");
  
  // ProfilingMode : Default, Counters or Timers
  m_cudaCompiler.define( "CR_PROFILING_MODE", ProfilingMode_Default); //
  
  m_CudaModule = m_cudaCompiler.compile();
  
  if (NULL == m_CudaModule) {
    exit( EXIT_FAILURE );
  }
  
  
  /// Setup CudaRaster
  
  m_cudaRaster.init();
  
  std::string pipePostfix = "passthrough";
  std::string kernelName( "vertexShader_" + pipePostfix );
  
  size_t paramSize = 1u * sizeof(FW::U32) + 
                     2u * sizeof(CUdeviceptr);
  
  m_vertexShaderKernel = m_CudaModule->getKernel( kernelName, paramSize);
  
  // specified every frame in Query::begin()..
  //m_cudaRaster.setSurfaces( m_colorBuffer, m_depthBuffer); // XXX

  std::string pipeName( "PixelPipe_" + pipePostfix );
  m_cudaRaster.setPixelPipe( m_CudaModule, pipeName);
}


void Context::objectToClipSpace(const ocull::Mesh &mesh, 
                                const ocull::Matrix4x4 &mvp)
{
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );
  
  /*
  // Set globals. (here, it can be seen as GLSL uniforms)
  FW::Constants& c = *(FW::Constants*)m_CudaModule->getGlobal(
                        "c_constants").getMutablePtrDiscard();
  
  // Translate glm matrix as CudaRaster Framework matrix
  SET( c.posToClip, mvp);//
  
  int ofs = 0;
  ofs += m_CudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_inVertices.getCudaPtr());
  ofs += m_CudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_outVertices.getMutableCudaPtrDiscard());
  ofs += m_CudaModule->setParami( m_vertexShaderKernel, ofs, m_numVertices);
  
  FW::Vec2i blockSize(32, 4);
  int numBlocks = (m_numVertices - 1) / (blockSize.x * blockSize.y) + 1;
  m_CudaModule->launchKernel(m_vertexShaderKernel, blockSize, numBlocks);
  */
}


void Context::setRasterizerParams(FW::CudaSurface *depthBuffer)
{
  assert( depthBuffer != NULL );
  
  
  if (m_colorBuffer != NULL) {
    delete m_colorBuffer;
  }
  
  m_colorBuffer = new FW::CudaSurface( depthBuffer->getSize(), 
                                       FW::CudaSurface::FORMAT_RGBA8, 
                                       1u);
  
  m_cudaRaster.setSurfaces( m_colorBuffer, depthBuffer);
  m_cudaRaster.deferredClear();
}

} //namespace ocull
