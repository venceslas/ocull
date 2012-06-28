///
///   \file ocullQuery.hpp
///
///   \note The way camera are stored may be modified later to let the users
///         store them ans send them to the context with begin()
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
    Camera m_camera;
    FW::CudaSurface *m_depthBuffer;
    Result m_result;
    
  
  public:
    Query(unsigned int width, unsigned int height);
    
    
    // +++ SETTERS +++
    
    void resize(unsigned int width, unsigned int height);
    
    inline void resize(const Vector2i &size) {resize(size.x, size.y);}
    
    inline void setCamera(const Camera &camera) {m_camera = camera;}
        
    /// Specify a default depth buffer, the size of the buffer must be the same
    /// as the query's depth buffer size.
    //void setDepthBuffer(..);
    
    
    // +++ GETTERS +++
    
    inline const ocull::Camera& getCamera() { return m_camera; }
    
    /// Return the depthbuffer size
    const ocull::Vector2i getSize();
    
    //getDepthBuffer();
    
    inline const Result& getResults() {return m_result;}
    
    
  private:
    void resetResults();
    
    
  // forbidden methods
  private:
    DISALLOW_COPY_AND_ASSIGN(Query);
};

} //namespace ocull

#endif //OCULL_QUERY_HPP_
