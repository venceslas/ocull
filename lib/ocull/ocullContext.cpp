
#include "ocullContext.hpp"
#include <cstdio>


namespace oql {

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
  fprintf( stderr, "%s : check paths validity. [TODO]\n" );
  

  /// Compiles the pipeline shaders
    
  m_cudaCompiler.include( "../src/framework" );
  m_cudaCompiler.include( "../src/" );
  m_cudaCompiler.setSourceFile( "shader/PassThrough.cu" );  
  
  // Compile CUDA code
  FW::U32 renderModeFlags = 0;
  if (App::kState.bDepth) renderModeFlags |= FW::RenderModeFlag_EnableDepth;
  if (App::kState.bLerp)  renderModeFlags |= FW::RenderModeFlag_EnableLerp;
  if (App::kState.bQuads) renderModeFlags |= FW::RenderModeFlag_EnableQuads;
  
  m_cudaCompiler.clearDefines();
  m_cudaCompiler.define( "SAMPLES_LOG2", m_colorBuffer->getSamplesLog2());//
  m_cudaCompiler.define( "RENDER_MODE_FLAGS", renderModeFlags);
  m_cudaCompiler.define( "BLEND_SHADER", "BlendReplace");
  
  // ProfilingMode : Default, Counters or Timers
  m_cudaCompiler.define("CR_PROFILING_MODE", ProfilingMode_Default); //
  
  m_cudaModule = m_cudaCompiler.compile();
  
  if (NULL == m_cudaModule) {
    exit( EXIT_FAILURE );
  }  
    
  
  /// Setup CudaRaster
  
  m_cudaRaster.init();
  
  std::string pipePostfix = "passthrough";
  
  std::string kernelName( "vertexShader_" + pipePostfix );
  size_t paramSize = 1 * sizeof(FW::U32) + 2 * sizeof(CUdeviceptr);
  m_vertexShaderKernel = m_cudaModule->getKernel( kernelName, paramSize);

  std::string pipeName( "PixelPipe_" + pipePostfix );
  
  //m_cudaRaster.setSurfaces( m_colorBuffer, m_depthBuffer); // XXX
  
  m_cudaRaster.setPixelPipe( m_cudaModule, pipeName);
}


void Query::objectToClipSpace(const ocull::Mesh &mesh, 
                              const ocull::Matrix4x4 mvp)
{
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );
  
  /*
  // Set globals. (here, it can be seen as GLSL uniforms)
  FW::Constants& c = *(FW::Constants*)m_cudaModule->getGlobal("c_constants"
                      ).getMutablePtrDiscard();
  
  // Translate glm matrix as CudaRaster Framework matrix
  SET( c.posToClip, mvp);//
  
  int ofs = 0;
  ofs += m_cudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_inVertices.getCudaPtr());
  ofs += m_cudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_outVertices.getMutableCudaPtrDiscard());
  ofs += m_cudaModule->setParami( m_vertexShaderKernel, ofs, m_numVertices);
  
  FW::Vec2i blockSize(32, 4);
  int numBlocks = (m_numVertices - 1) / (blockSize.x * blockSize.y) + 1;
  m_cudaModule->launchKernel(m_vertexShaderKernel, blockSize, numBlocks);
  */
}

} //namespace oql
