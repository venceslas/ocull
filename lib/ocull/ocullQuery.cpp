///
///
///
///

#include "ocullQuery.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include "rasterizer/CudaSurface.hpp"
#include "ocullScene.hpp"



namespace ocull {


Query::~Query()
{
  if (m_DepthBuffer)
  {
    delete m_DepthBuffer;
    m_DepthBuffer = NULL;
  }
}


///----------------------


void Query::resize(unsigned int w, unsigned int h)
{
  // Check if the new resolution is valid
  if (((w%CR_TILE_SIZE)!=0) || ((h%CR_TILE_SIZE)!=0))
  {
    std::cerr << "[Error " << __FUNCTION__ << "] "
              << "The depth buffer resolution must be a multiple of "
              << CR_TILE_SIZE
              << std::endl;
    return;
  }
  
  // Recreate the buffer only if it has changed
  if (m_DepthBuffer != NULL)
  {
    FW::Vec2i bufferSize = m_DepthBuffer->getSize();
    
    if ((bufferSize.x == w) && (bufferSize.y == h)) {
      return;
    }
    
    delete m_DepthBuffer;
  }
  
  m_DepthBuffer = new FW::CudaSurface( FW::Vec2i(w, h), 
                                       FW::CudaSurface::FORMAT_DEPTH32 );
}


///----------------------


void Query::begin(const ocull::Camera &camera, 
                  const ocull::DepthBuffer *pDepthBuffer)
{
  assert( !m_bHasBegun );  
  
  
  // Set the camera used for the current query
  m_pCamera = &camera;
  
  // Clears the previous query results
  resetStats();
  
  // Clears the depth buffer or set a default one
  //clearDepth( pDepthBuffer );
  
  // Set Depth buffer surface, default content (if any)
  m_context.setRasterizerParams( m_DepthBuffer ); 
  
  //m_context.setRasterizerQueryParams();
  
  
  m_bHasBegun = true;
}

void Query::end()
{
  assert( m_bHasBegun );
  
  m_pCamera = NULL;  
  m_bHasBegun = false;
}


///----------------------


void Query::uploadScene(ocull::Scene &scene)
{
  assert( m_bHasBegun );
  
  fprintf( stderr, "%s : not implemented yet\n", __FUNCTION__);
  
}


void Query::uploadMesh(ocull::Mesh &mesh, 
                       const ocull::Matrix4x4 &worldMatrix)
{
  assert( m_bHasBegun );
  
  
  /// Update stats
  m_stats.objectCount += 1u;
  m_stats.trianglePassedCount += mesh.getTriangleCount();//
  
  // TODO: precompute the view proj
  Matrix4x4 mvp = m_pCamera->frustum.projectionMatrix * 
                  m_pCamera->viewMatrix *
                  worldMatrix;
  
  /// Send to the rasterizer
  m_context.uploadMesh( mesh, mvp);  
}
                        

///----------------------



void Query::getSamplesPassed(unsigned int *samplesPassed)
{
  assert( !m_bHasBegun );  
  
  if (m_stats.samplesPassed.empty()) {
    *samplesPassed = 0u;
    return;
  }
  
  unsigned int total = 0u;
  for (size_t i=0u; i<m_stats.samplesPassed.size(); ++i) {
    total += m_stats.samplesPassed[i];
  }
  
  *samplesPassed = total;
}

void Query::getSamplesPassed(unsigned int *samplesPassed, size_t count)
{
  assert( !m_bHasBegun );
   
  if (m_stats.objectCount == 0u) {
    return;
  }
  
  if (samplesPassed == NULL) {
    return;
  }
  
  const size_t numElems = std::min( count, m_stats.samplesPassed.size());
  memcpy( samplesPassed, &m_stats.samplesPassed[0], numElems * sizeof(unsigned int));
}
    
void Query::getDepthBuffer(ocull::DepthBuffer *depthBuffer)
{  
  assert( m_DepthBuffer != NULL );
  
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );
  
  /*
  if (depthBuffer == NULL) {
    return;
  }
  
  // TODO
  */
}


void Query::clearDepth(const ocull::DepthBuffer *pDepthBuffer)
{
  /*
  assert( m_DepthBuffer != NULL );
    
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );
  
  // XXX should use a CUDA kernel for this
   
  if (pDepthBuffer == NULL)
  {
    // clear the depth buffer
  }
  else
  {
    // fill the depth buffer with values from pDepthBuffer    
    // TODO assert the format are the sames
  }
*/
}

void Query::resetStats()
{
  if (!m_stats.samplesPassed.empty()) {
    m_stats.samplesPassed.assign( m_stats.samplesPassed.size(), 0u);
  }
  
  m_stats.objectCount = 0u;
  m_stats.objectPassedCount = 0u;
  m_stats.triangleCount = 0u;
  m_stats.trianglePassedCount = 0u;
  m_stats.queryTime = 0.0f;
}

} // namespace ocull
