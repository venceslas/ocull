///
///   \file ocullContext.cpp
///

#include "ocullContext.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "rasterizer/framework/gpu/CudaModule.hpp"
#include "rasterizer/shader/PassThrough.hpp"       // kernel shader
#include "ocullQuery.hpp"
#include "ocullScene.hpp"

#include <cudaGL.h>


namespace ocull {


// STATIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

Context* Context::Create(const std::string &pipeCubinFile)
{
  Context *context = new Context();
  
  
  /// Load the precompiled pipeline shaders
  FW::CudaModule *module = new FW::CudaModule(pipeCubinFile); 
  
  if (NULL == module) 
  {
    std::cerr << __FUNCTION__ 
              << " : invalid argument \""  << pipeCubinFile << "\"."
              << std::endl;
    return NULL;
  }  
  context->m_cudaModule = module;
  
  
  /// Initialized CudaRaster  
  context->m_rasterizer.init();
  
  // Vertex shader
  size_t paramSize = 1u * sizeof(FW::U32) + 2u * sizeof(CUdeviceptr);
  context->m_vsKernel = module->getKernel( "vertexShader_passthrough", paramSize);
  
  // Pipeline
  context->m_rasterizer.setPixelPipe( module, "PixelPipe_passthrough");
  
  
  return context;
}


void Context::Release(Context* context)
{
  assert( context != NULL );
  OCULL_SAFEDELETE( context );
}
    


// PUBLIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

void Context::begin(Query *pQuery)
{
  assert( isQueryBounded() == false );
  assert( pQuery != NULL );
  
  
  /// Set rasterizer parameters
  // -----
  // [TEMPORARY] Reset the color buffer (costly !)  
  
  FW::Vec2i depthSize = pQuery->m_depthBuffer->getSize();
  
  if ((m_colorBuffer != NULL) && (depthSize != m_colorBuffer->getSize())) {
    OCULL_SAFEDELETE(m_colorBuffer);
  }
  
  if (m_colorBuffer == NULL) {
    m_colorBuffer = new FW::CudaSurface( depthSize, FW::CudaSurface::FORMAT_RGBA8, 1u);
  }
  
  // Specify the color (TMP) and depth buffer.
  m_rasterizer.setSurfaces( m_colorBuffer, pQuery->m_depthBuffer);
  m_rasterizer.deferredClear();
    
  // TODO : fill the depth buffer with the query default depthbuffer (if any)
  // -----
    
  /// Reset query results
  pQuery->resetResults();
  
  /// Bound the query
  m_pQuery = pQuery;
}


void Context::end()
{
  assert( isQueryBounded() == true );
  
  /// Unbound the query
  m_pQuery = NULL;
}


void Context::uploadScene(ocull::Scene *pScene)
{
  /// It could be improve furthermore, sending buffer of MVPs and proceed
  /// everything in less Kernels. (Just an idea to explore)
  
  assert( isQueryBounded() == true );
  assert( pScene != NULL );
  
  
  std::cerr << __FUNCTION__ << " : unfinished." << std::endl;
  
# if OCULLSCENE_CACHE_FRIENDLY

# else

# endif
}


void Context::uploadMesh(ocull::Mesh *pMesh, const ocull::Matrix4x4 &modelMatrix)
{
  assert( isQueryBounded() == true );
  assert( pMesh != NULL );
  
  
  /// Update simple stats
  Query::Result &result = m_pQuery->m_result; //
  result.objectCount += 1u;
  result.trianglePassedCount += pMesh->getTriangleCount();
    
  
  /// Resize the output vertices buffer
  size_t vertexSize = pMesh->vertex.count * sizeof(FW::ShadedVertex_passthrough);
  m_outVertices.resizeDiscard( vertexSize );  //
  
  // Vertex Shader
  runVertexShader( pMesh, modelMatrix);  
  
  
  // Pipeline
  m_rasterizer.setVertexBuffer( &m_outVertices, 0);
  m_rasterizer.setIndexBuffer( &(pMesh->index.buffer), 
                               pMesh->index.offset, 
                               pMesh->getTriangleCount());
  //m_rasterizer.setIndexBuffer_TEST( &pDevice, pMesh->getTriangleCount());
  
  m_rasterizer.drawTriangles();
}

// [DEBUG only] (to remove later)
unsigned int Context::getColorTexture()
{
  assert( m_colorBuffer != NULL );
  return m_colorBuffer->getGLTexture();
}


// PRIVATE methods +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


Context::Context()
    : m_cudaModule(NULL),
      m_pQuery(NULL),
      m_colorBuffer(NULL)
{
}


Context::~Context()
{
  OCULL_SAFEDELETE( m_cudaModule )
  OCULL_SAFEDELETE( m_pQuery )
  OCULL_SAFEDELETE( m_colorBuffer ) //tmp  
}


void Context::runVertexShader( ocull::Mesh *pMesh, const ocull::Matrix4x4 &modelMatrix)
{
  /// Map constants parameters (like GLSL's uniforms)
  FW::Constants& c = *(FW::Constants*)
                      m_cudaModule->getGlobal("c_constants").getMutablePtrDiscard();


  // Compute the ModelViewProjection matrix
  const Matrix4x4 &viewProj = m_pQuery->m_camera.getViewProjMatrix();
  Matrix4x4 mvp = viewProj * modelMatrix;

  // Translate custom matrix as CudaRaster Framework matrix (yeah.. what ?)
# define COPY_MAT(i,j)  c.posToClip.m##i##j = mvp[j][i]
  COPY_MAT(0, 0); COPY_MAT(0, 1); COPY_MAT(0, 2); COPY_MAT(0, 3);
  COPY_MAT(1, 0); COPY_MAT(1, 1); COPY_MAT(1, 2); COPY_MAT(1, 3);
  COPY_MAT(2, 0); COPY_MAT(2, 1); COPY_MAT(2, 2); COPY_MAT(2, 3);
  COPY_MAT(3, 0); COPY_MAT(3, 1); COPY_MAT(3, 2); COPY_MAT(3, 3);
# undef COPY_MAT

  //-------
    
  /// Set input parameters
  int ofs = 0;
  ofs += m_cudaModule->setParamPtr( m_vsKernel, ofs, pMesh->vertex.buffer.getCudaPtr());
  ofs += m_cudaModule->setParamPtr( m_vsKernel, ofs, m_outVertices.getMutableCudaPtrDiscard());
  ofs += m_cudaModule->setParami  ( m_vsKernel, ofs, pMesh->vertex.count);  
  // XXX TODO set vertex stride & offset XXX

  //-------
  
  /// Run the vertex shader kernel
  FW::Vec2i blockSize(32, 4);
  int numBlocks = (pMesh->vertex.count - 1u) / (blockSize.x * blockSize.y) + 1;
  m_cudaModule->launchKernel( m_vsKernel, blockSize, numBlocks);
}

} //namespace ocull
