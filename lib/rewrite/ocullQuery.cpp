///
///   \file ocullQuery.cpp
///   
///

#include "ocullQuery.hpp"

#include <iostream>
#include "rasterizer/CudaSurface.hpp"



namespace ocull {


// PUBLIC methods  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


Query::Query(unsigned int width, unsigned int height)
    : m_pCamera(NULL),
      m_depthBuffer(NULL)
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
  if (m_depthBuffer != NULL)
  {
    FW::Vec2i bufferSize = m_depthBuffer->getSize();    
    
    if ((bufferSize.x == width) && (bufferSize.y == height)) {
      return;
    }
    
    delete m_depthBuffer;
  }
  
  m_DepthBuffer = new FW::CudaSurface( FW::Vec2i(w, h), 
                                       FW::CudaSurface::FORMAT_DEPTH32 );
}

//setCamera()
//setDepthBuffer()



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
    
};

} //namespace ocull
