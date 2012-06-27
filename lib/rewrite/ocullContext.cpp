///
///   \file ocullContext.cpp
///

#include "ocullContext.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "ocullQuery.hpp"
#include "ocullScene.hpp"



namespace ocull {


// STATIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


Context::Context* Create(const std::string &pipeCubinFile)
{
  Context *context = new Context();
  
  
  /// Load the precompiled pipeline shaders
  
  m_cudaModule = new CudaModule(pipeCubinFile);
  
  if (NULL == m_cudaModule) 
  {
    std::cerr << __FUNCTION__ 
              << " : invalid argument \""  << pipeCubinFile << "\"."
              << std::endl;
    return NULL;
  }
    
  
  /// Initialized CudaRaster
  
  m_rasterizer.init();
  
  // Vertex shader
  size_t paramSize = 1u * sizeof(FW::U32) + 2u * sizeof(CUdeviceptr);
  m_vsKernel = m_CudaModule->getKernel( "vertexShader_passthrough", paramSize);
  
  // Pipeline
  m_rasterizer.setPixelPipe( m_CudaModule, "PixelPipe_passthrough");
  
  
  return context;
}


void Context::Release(Context* context)
{
  assert( context != NULL );
  delete context;
}
    


// PUBLIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


void Context::begin(Query *pQuery)
{
  assert( isQueryBounded() == false );
  assert( pQuery != NULL );
  
  
  /// Set rasterizer parameters
  
  //---  
  // [TEMPORARY] Reset the color buffer (costly !)
  
  FW::Vec2i bufferSize = pQuery->m_depthBuffer->getSize();
      
  if (m_colorBuffer != NULL) {
    if ((bufferSize.x != width) || (bufferSize.y != height)) {
      delete m_colorBuffer;
    }
  }  
  m_colorBuffer = new FW::CudaSurface( bufferSize, 
                                       FW::CudaSurface::FORMAT_RGBA8, 
                                       1u);
  
  // TODO : fill the depth buffer with the query default depthbuffer (if any)
  
  // Specify the color (TMP) and depth buffer.
  m_rasterizer.setSurfaces( m_colorBuffer, pQuery->m_depthBuffer);
  m_rasterizer.deferredClear();
  //---
  
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
  /// everything in less Kernels. (Just an idea)
  
  assert( isQueryBounded() == true );
  assert( pScene != NULL );
  
  
  std::cerr << __FUNCTION__ << " : unfinished." << std::endl;
  
# if 1   // no cache efficient version

# else   // (supposed) cache efficient

# endif
}


void Context::uploadMesh(ocull::Mesh *pMesh, const ocull::Matrix4x4 &modelMatrix)
{
  assert( isQueryBounded() == true );
  assert( pMesh != NULL );
  
  
  /// Update simple stats
  Query::Result &result = m_pQuery->result;
  result.objectCount += 1u;
  result.trianglePassedCount += pMesh->getTriangleCount();
  
  /// Compute the ModelViewProjection matrix
  const Matrix4x4 &viewProj = m_pQuery->camera.getViewProjMatrix();
  Matrix4x4 mvp = viewProj * modelMatrix;
    
  
  // resize the output vertices buffer
  size_t csVertexSize = mesh.vertex.size * sizeof(FW::ShadedVertex_passthrough);
  m_csVertices.resizeDiscard( csVertexSize );  
    
  // Map constants (kind of GLSL's uniforms) parameters
  FW::Constants& c = *(FW::Constants*)
                      m_CudaModule->getGlobal("c_constants").getMutablePtrDiscard();
  
  // Translate custom matrix as CudaRaster Framework matrix (yeah.. what ?)
# define COPY_MAT(i,j)  c.posToClip.m##i##j = mvp[j][i]
  COPY_MAT(0, 0); COPY_MAT(0, 1); COPY_MAT(0, 2); COPY_MAT(0, 3);
  COPY_MAT(1, 0); COPY_MAT(1, 1); COPY_MAT(1, 2); COPY_MAT(1, 3);
  COPY_MAT(2, 0); COPY_MAT(2, 1); COPY_MAT(2, 2); COPY_MAT(2, 3);
  COPY_MAT(3, 0); COPY_MAT(3, 1); COPY_MAT(3, 2); COPY_MAT(3, 3);
# undef COPY_MAT
  
  
  std::cerr << __FUNCTION__ << " : unfinished." << std::endl;
  
  // Not sure at all for this, I think I have to sent the vbo's with getGLPtr() or something
  /**
  int ofs = 0;
  ofs += m_cudaModule->setParamPtr( m_vsKernel, ofs, mesh.vertex.buffer.getCudaPtr());
  ofs += m_cudaModule->setParamPtr( m_vsKernel, ofs, m_csVertices.getMutableCudaPtrDiscard());
  ofs += m_cudaModule->setParami( m_vsKernel, ofs, mesh.vertex.size);
  // TODO set vertex stride & offset
  
  FW::Vec2i blockSize(32, 4);
  int numBlocks = (mesh.vertex.size - 1) / (blockSize.x * blockSize.y) + 1;
  m_cudaModule->launchKernel( m_vsKernel, blockSize, numBlocks);
  
  /// Sent transformed vertices to the pipeline
  m_rasterizer.setVertexBuffer( &m_csVertices, 0);
  m_rasterizer.setIndexBuffer( &mesh.index.buffer, mesh.index.offset, mesh.getTriangleCount());
  m_rasterizer.drawTriangles();
  */
}


// [DEBUG only] (to remove later)
unsigned int Context::getGLTexture()
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


} //namespace ocull
