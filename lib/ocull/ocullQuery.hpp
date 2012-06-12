///
///   \file ocullQuery.hpp
///
///

#ifndef OCULL_QUERY_HPP_
#define OCULL_QUERY_HPP_

#include "ocullDefs.hpp"
#include "ocullRasterizer.hpp"


namespace Ocull {

class Scene;


///
///   Maps a client side array of 32bits integer.
///   This is used for the client to retrieve the list of visible objects index.
///
class IndexList
{
  private:
    int *m_pArray;
    int m_size;
    
  public:
    IndexList(int *pArray, int size)
        : m_pArray(pArray),
          m_size(size)
    {}
    
    int* getPtr() const;
    int getSize() const;

  private:
    DISALLOW_COPY_AND_ASSIGN(IndexList);
};

///
///  
///
class OcclusionBuffer
{
  public:
    struct BufferDesc
    {
      int width;
      int height;
    };
    
  private:
    BufferDesc m_desc;
    //FW::CudaSurface* m_depthBuffer;
    
  public:
    OcclusionBuffer();
  
    bool isAABBVisible( const Vector3 &min, const Vector3 &max);
    
    void getBuffer(unsigned char *buffer) const;    
    void getBufferDesc(BufferDesc& desc) const;
  

  private:
    DISALLOW_COPY_AND_ASSIGN(OcclusionBuffer);
};

///
/// 
///
class Query
{
  private:
    static Ocull::Rasterizer sRasterizer;
    
  private:
    const Ocull::Scene *m_pScene;
    //use 'rasterizer'
          
  public:
    Query();
    ~Query();
    
    void init(const Scene *scene);
    
    void queryVisibility( const Ocull::Camera& camera, 
                          Ocull::IndexList* pIndexList,
                          OcclusionBuffer* pOcclusionBuffer=NULL);

  private:
    DISALLOW_COPY_AND_ASSIGN(Query);
};

}

#endif // OCULL_QUERY_HPP_
