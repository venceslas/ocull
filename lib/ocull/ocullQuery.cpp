
#include "Query.hpp"
#include <cassert>


namespace oql {


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
    const FW::Vec2i bufferRes = m_DepthBuffer.getSize();
    
    if ((bufferRes.x == w) && (bufferRes.y == h)) {
      return;
    }
    
    delete m_DepthBuffer;
  }
    
  m_DepthBuffer = new FW::CudaSurface( FW::Vec2i(w, h), FW::CudaSurface::FORMAT_DEPTH32 );
}


///----------------------


void Query::begin(const ocull::Camera &camera, 
                  const ocull::DepthBuffer *pDepthBuffer)
{
  assert( !m_bBegun );
  assert( m_DepthBuffer != NULL );
  
  
  m_pCamera = &camera;
  
  clearDepth(pDepthBuffer);
  resetStats();

  //m_context.rasterizer.setDepthBuffer( m_depthBuffer );
  //m_context.rasterizer.deferredClear();// check this
  
  //m_context.rasterizer.setQueryParams();
  
  m_bBegun = true:
}

void Query::end()
{
  assert( m_bBegun );
  m_bBegun = false;
}


///----------------------


void Query::uploadScene(const ocull::Scene &scene)
{
  assert( m_bBegun );
  
  
}


void Query::uploadMesh(const ocull::Mesh &mesh, 
                       const ocull::Matrix4x4 &worldMatrix)
{
  assert( m_bBegun );
  
  
  /// Update stats
  m_stats.numObjects += 1u;
  m_stats.numTriangles += mesh.getNumTriangles();//
  
  
  /// Transform to clip space TODO  
  rasterizer.objectToClipSpace(..); //
  
    
  /// Send to the rasterizer
  //rasterizer.setVertexBuffer( &vertices, 0);
  rasterizer.setIndexBuffer( &indices, 0, numTriangles);
  rasterizer.drawTriangles();
}
                        


///----------------------


void Query::getSamplesPassed(unsigned int *samplesPassed)
{
  assert( !m_bBegun );
  
  
  if (m_stats.samplesPassed.empty())
  {
    *samplesPassed = 0u;
    return;
  }
  
  unsigned int total = 0u;
  for (size_t i=0u; i<m_stats.samplesPassed.size(); ++i)
  {
    total += m_stats.samplesPassed[i];
  }
  
  *samplesPassed = total;
}

void Query::getSamplesPassed(unsigned int *samplesPassed, size_t count)
{
  assert( !m_bBegun );
   
  
  if (m_stats.numObjects == 0u) {
    return;
  }
    
  if (samplesPassed == NULL) {
    return;
  }
  
  const size_t numElems = min( count, m_stats.samplesPassed.size());  
  memcpy( samplesPassed, &m_stats.samplesPassed[0], numElems * sizeof(unsigned int));
}
    
void Query::getDepthBuffer(ocull::DepthBuffer *depthBuffer)
{
  assert( m_DepthBuffer != NULL );
    
  fprintf( stderr, "%s not implemented yet\n", __FUNCTION__ );
  
  
  if (depthBuffer == NULL) {
    return;
  }
  
  // TODO
}

} // namespace oql


void Query::clearDepth(const ocull::DepthBuffer *pDepthBuffer)
{
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
}

void Query::resetStats()
{
  if (!m_stats.samplesPassed.empty()) {
    m_stats.samplesPassed.assign( m_stats.samplesPassed.size(), 0u);
  }
  
  m_stats.numObjects = 0u;
  m_stats.numObjectsPassed = 0u;
  m_stats.numTriangles = 0u;
  m_stats.numTrianglesPassed = 0u;
  m_stats.queryTime = 0.0f;
}

} //namespace oql
