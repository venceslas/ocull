///
///   \file ocullQuery.cpp
///
///

#include "ocullQuery.hpp"


namespace Ocull {


// ----------------------
// OcclusionBuffer
// ----------------------  

bool OcclusionBuffer::isAABBVisible(const Vector3 &min, const Vector3 &max)
{
  return false;
}


// ----------------------
// Query
// ----------------------  

Query::Query()
{  
}

Query::~Query()
{
}

void Query::init(const Scene *scene)
{
  m_pScene = scene;
  
  //sRasterizer.init();
}


void Query::queryVisibility(const Camera& camera, 
                            IndexList* pIndexList,
                            OcclusionBuffer* pOcclusionBuffer)
{
  /*
  for (int i=0; i<m_pScene->getObjectCount(); ++i) {
    sRasterizer.render( camera, m_pScene->getObject(i) );
  }
  */
}

}

