///
///   \file ocullQuery.cpp
///   
///

#include "ocullQuery.hpp"

#include <iostream>
#include "rasterizer/CudaSurface.hpp"
#include "rasterizer/cuda/Constants.hpp"  // for CR_TILE_SIZE



namespace ocull {


// PUBLIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

Query::Query(unsigned int width, unsigned int height)
    : m_depthBuffer(NULL)
{
  resize( width, height);
  resetResults();
}


void Query::resize(unsigned int width, unsigned int height)
{
  // Check if the new resolution is valid
  if (((width%CR_TILE_SIZE)!=0) || ((height%CR_TILE_SIZE)!=0))
  {
    std::cerr << "[Error " << __FUNCTION__ << "] "
              << "The depth buffer resolution must be a multiple of "
              << CR_TILE_SIZE
              << std::endl;
    return;
  }
  
  // Recreate the buffer only if it has changed
  FW::Vec2i bufferSize( width, height);
  
  if (m_depthBuffer != NULL)
  {    
    if (bufferSize == m_depthBuffer->getSize()) {
      return;
    }    
    delete m_depthBuffer;
  }
  
  m_depthBuffer = new FW::CudaSurface( bufferSize, 
                                       FW::CudaSurface::FORMAT_DEPTH32 );
}

//setCamera()

//setDepthBuffer()


const ocull::Vector2i Query::getSize()
{
  FW::Vec2i size = m_depthBuffer->getSize();
  return Vector2i( size.x, size.y);
}


// PRIVATE methods +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

void Query::resetResults()
{
  if (!m_result.samplesPassed.empty()) {
    m_result.samplesPassed.assign( m_result.samplesPassed.size(), 0u);
  }
  
  m_result.objectCount = 0u;
  m_result.objectPassedCount = 0u;
  m_result.triangleCount = 0u;
  m_result.trianglePassedCount = 0u;
  m_result.queryTime = 0.0f;
}
    
} //namespace ocull
