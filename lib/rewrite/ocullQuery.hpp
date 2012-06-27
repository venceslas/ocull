///
///   \file ocullQuery.hpp
///   
///

#ifndef OCULL_QUERY_HPP_
#define OCULL_QUERY_HPP_

#include <vector>

#include "ocullDefs.hpp"
#include "ocullCamera.hpp"

namespace FW { 
class CudaSurface; 
}


namespace ocull {

class Context;

class Query
{
  friend class Context;//
  
  public:
    struct Result
    {    
      std::vector<unsigned int> samplesPassed;
      unsigned int objectCount;
      unsigned int objectPassedCount;
      unsigned int triangleCount;
      unsigned int trianglePassedCount;      
      float queryTime;
    };
  
  
  private:
    Camera *m_pCamera;//
    FW::CudaSurface *m_depthBuffer;
    
    Result m_result;
    
  
  public:
    Query(unsigned int width, unsigned int height);
  
    void resize(unsigned int width, unsigned int height);
    inline void resize(const Vector2i size) {resize(size.x, size.y);}
    
    //setCamera()
    //setDepthBuffer()
    
    // +++ GETTERS +++
    //
    
  private:
    void resetResults();
    
  // forbidden methods
  private:
    DISALLOW_COPY_AND_ASSIGN(Query);
};

} //namespace ocull

#endif //OCULL_QUERY_HPP_
