///
///
///
///

#include "ocullContext.hpp"
#include <cstdio>
#include <cstring>

#include "ocullScene.hpp"
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
  fprintf( stderr, "%s : check paths validity !! [TODO]\n", __FUNCTION__ );
  
  
  /// Compiles the pipeline shaders
  
  m_cudaCompiler.include( "../../lib/ocull/rasterizer" );
  m_cudaCompiler.include( "../../lib/ocull/rasterizer/framework" );  
  m_cudaCompiler.setSourceFile( "../../lib/ocull/rasterizer/shader/PassThrough.cu" );  
  
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
  
  std::string pipeName( "PixelPipe_" + pipePostfix );
  m_cudaRaster.setPixelPipe( m_CudaModule, pipeName);
  
  // depth surface is specified by the query
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


void Context::uploadMesh(ocull::Mesh &mesh, const ocull::Matrix4x4 &mvp)
{
  // resize the output vertices buffer
  size_t csVertexSize = mesh.vertex.size * sizeof(FW::ShadedVertex_passthrough);
  m_csVertices.resizeDiscard( csVertexSize );
  
  
  /// Transform to clip space (a.k.a. the Vertex Shader)
  // Set globals. (here, it can be seen as GLSL uniforms)
  FW::Constants& c = *(FW::Constants*)m_CudaModule->getGlobal("c_constants").getMutablePtrDiscard();
  
  // Translate glm matrix as CudaRaster Framework matrix
#define COPY_MAT(i,j)  c.posToClip.m##i##j = mvp[j][i]
  COPY_MAT(0, 0); COPY_MAT(0, 1); COPY_MAT(0, 2); COPY_MAT(0, 3);
  COPY_MAT(1, 0); COPY_MAT(1, 1); COPY_MAT(1, 2); COPY_MAT(1, 3);
  COPY_MAT(2, 0); COPY_MAT(2, 1); COPY_MAT(2, 2); COPY_MAT(2, 3);
  COPY_MAT(3, 0); COPY_MAT(3, 1); COPY_MAT(3, 2); COPY_MAT(3, 3);
#undef COPY_MAT
  
  // Not sure at all for this, I think I have to sent the vbo's with getGLPtr() or something
  
  /*
  int ofs = 0;
  ofs += m_CudaModule->setParamPtr( m_vertexShaderKernel, ofs, mesh.vertex.buffer.getCudaPtr());
  ofs += m_CudaModule->setParamPtr( m_vertexShaderKernel, ofs, m_csVertices.getMutableCudaPtrDiscard());
  ofs += m_CudaModule->setParami( m_vertexShaderKernel, ofs, mesh.vertex.size);
  // TODO set vertex stride & offset
  
  FW::Vec2i blockSize(32, 4);
  int numBlocks = (mesh.vertex.size - 1) / (blockSize.x * blockSize.y) + 1;
  m_CudaModule->launchKernel( m_vertexShaderKernel, blockSize, numBlocks);
  
  /// Sent transformed vertices to the pipeline
  m_cudaRaster.setVertexBuffer( &m_csVertices, 0);
  m_cudaRaster.setIndexBuffer( &mesh.index.buffer, mesh.index.offset, mesh.getTriangleCount());
  m_cudaRaster.drawTriangles();
  */
}

} //namespace ocull
