///
///   /file ocullRasterizer.hpp
///


#include "ocullRasterizer.hpp"


namespace Ocull {
    
Rasterizer::~Rasterizer()
{
  if (m_depthBuffer)
  {
    delete m_depthBuffer;
    m_depthBuffer = NULL;
  }
}

void Rasterizer::init()
{  
  m_cudaCompiler.include( "src/rasterizer/" );
  m_cudaCompiler.include( "src/rasterizer/framework/" );  
  m_cudaCompiler.setSourceFile( "src/rasterizer/shader/PassThrough.cu" );
  
  m_cudaRaster.init();
  
  initPipe();  
}

void Rasterizer::initPipe()
{
#if 0
  // Create surface
  FW::Vec2i screenResolution = FW::Vec2i( App::kScreenWidth, App::kScreenHeight); //
  
  if (m_depthBuffer) delete m_depthBuffer;
  m_depthBuffer = new FW::CudaSurface( screenResolution, FW::CudaSurface::FORMAT_DEPTH32, 1);
  
  
  // Compile CUDA code.
  FW::U32 renderModeFlags = FW::RenderModeFlag_EnableDepth |
                            FW::RenderModeFlag_EnableLerp  |
                            FW::RenderModeFlag_EnableQuads;
  
  m_cudaCompiler.clearDefines();
  m_cudaCompiler.define( "SAMPLES_LOG2", m_colorBuffer->getSamplesLog2());//
  m_cudaCompiler.define( "RENDER_MODE_FLAGS", renderModeFlags);//
  m_cudaCompiler.define( "BLEND_SHADER", "BlendReplace" );
  
  // ProfilingMode : Default, Counters or Timers
  m_cudaCompiler.define( "CR_PROFILING_MODE", ProfilingMode_Default); //
  
  m_cudaModule = m_cudaCompiler.compile();
  
  if (NULL == m_cudaModule) {
    exit( EXIT_FAILURE );
  }

  
  // Setup CudaRaster.  
  std::string pipePostfix = "passthrough";
  
  std::string kernelName( "vertexShader_" + pipePostfix );
  size_t paramSize = 1 * sizeof(FW::U32) + 2 * sizeof(CUdeviceptr);
  m_vertexShaderKernel = m_cudaModule->getKernel( kernelName, paramSize);

  std::string pipeName( "PixelPipe_" + pipePostfix );
  m_cudaRaster.setSurfaces( m_colorBuffer, m_depthBuffer);
  m_cudaRaster.setPixelPipe( m_cudaModule, pipeName);
#endif
}



void Rasterizer::render( const Ocull::Camera& camera, const Ocull::Object& obj)
{
#if 0
  /*
  FW::Vec2i windowSize( App::kScreenWidth, App::kScreenWidth);
  
  // Parameters changed => reinitialize pipe.
  if (m_colorBuffer && m_colorBuffer->getSize() != windowSize) {
    printf("reinit PIPE : resize buffer\n");
    m_pipeDirty = true;
  }
  */
  
  if (m_pipeDirty) {
    initPipe();
  }
  m_pipeDirty = false;
  
  
  //----------------------------------
    
  /// ========== 1) Custom VertexShader to transform the vertices ==========
  
  // Set globals. (ie, GLSL uniforms)
  FW::Constants& c = *(FW::Constants*)m_cudaModule->getGlobal("c_constants").getMutablePtrDiscard();
  
  // Translate glm matrix as CudaRaster Framework matrix
  glmToFW_matrix4f( camera.getViewProjMatrix(), c.posToClip);
  
  // 
  int ofs = 0;
  ofs += m_cudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_inVertices.getCudaPtr());
  ofs += m_cudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_outVertices.getMutableCudaPtrDiscard());
  ofs += m_cudaModule->setParami( m_vertexShaderKernel, ofs, m_numVertices);
  
  FW::Vec2i blockSize( 32, 4);
  int numBlocks = (m_numVertices - 1) / (blockSize.x * blockSize.y) + 1;
  m_cudaModule->launchKernel(m_vertexShaderKernel, blockSize, numBlocks);
  
  
  /// ========== 2) Run CudaRaster ==========

  m_cudaRaster.deferredClear( FW::Vec4f(0.0f, 0.0f, 0.0f, 1.0f) );  
  
  m_cudaRaster.setVertexBuffer( &m_outVertices, 0);
  m_cudaRaster.setIndexBuffer( &m_indices, 0, m_numTriangles);
    
  m_cudaRaster.drawTriangles();
#endif
}

}
