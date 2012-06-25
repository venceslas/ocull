///
///   Simple Occlusion Query Library
///
/// Ce fichier a pour but de décrire l'utilisation de la librairie
/// de query 'simple'.
/// Dans la version simple, les queries sont effectués sur la géométrie
/// utilisée pour le rendu (partage des vertex buffers) et aucune étape
/// de preprocessing n'est effectuée sur les données.
/// Les paramètres d'entrées sont :
///   # une camera (frustum + viewMatrix) munie ou non d'un DepthBuffer
/// par défaut.
///   # un ensemble données géométrique représentant la scène à rendre.
/// 
///
/// Datas to retrieve :
///   # number of samples passed per MODEL
///       => retrieve an array of numSamplesPassed
///
///   # resulting depth buffer
///       => retrieve the FUCKING DEPTH BUFFER
///


#include <cstdio>
#include <ocull/ocull.hpp>


ocull::Camera g_camera;
ocull::DepthBuffer g_scene;
ocull::Scene g_scene;
ocull::Query *g_Query;

void frame_query();


int main(void)
{
  /// IN BEFORE
  
  // window's context, CUDA init, ..
  ENGINE.init();
  

  /// A - Init -------------
  
  /// 1) Context & Query
  ocull::Context ocullContext;
    
  ocullContext.init();
  
  //g_Query = new ocull::Query( ocullContext );
  g_Query = ocullContext.createQuery();  
  
    
  /// 2) Camera  
  g_camera.frustum = ocull::Frustum( _fov, _aspectRatio, _zNear, _zFar);
  g_camera.viewMatrix = ENGINE.CAMERA.getViewMatrix();   
  
  // optionally set a default depth buffer
  g_depth = ocull::DepthBuffer( ENGINE.getDepthBuffer() );  
  
      
  /// 3) Scene  
  // geometry
  for (size_t i=0u; i<ENGINE.getNumMesh(); ++i) {
    g_scene.insertMesh( ocull::Mesh(ENGINE.model[i].getMesh()) );
  }
  
  // instances (position)
  for (size_t i=0u; i<ENGINE.getNumModel(); ++i)
  {
    unsigned int meshId = ENGINE.model[i].getMeshId();
    g_scene.insertModel( meshId, ENGINE.model[i].getWorldMatrix());
  }
  
  
  /// B - mainloop -------------
  while (true)
  {
    //..    
    frame_query();    
    //..
  }
  
  //delete g_Query;
  ocullContext.releaseQuery( g_Query )

  return EXIT_SUCCESS;
}


void frame_query();
{
  /// Query's per-frame parameters update
  
  // update the view matrix
  g_Camera.viewMatrix = ENGINE.getViewMatrix();
  

  //---
  
  /// Perform query
  
  g_Query->begin( g_camera, &g_depth);
    // Upload a pre-packed scene (eg, for static geometry)
    g_Query->uploadScene( g_scene );
    
    // Upload 'by hand' (for dynamic geometry)
    g_Query->uploadMesh( Mesh(..), modelMatrix);
  g_Query->end();
  
    
  //---

  /// Retrieve query params
  
  // total samples passed
  unsigned int totalSamplesPassed;
  g_Query->getSamplesPassed( &totalSamplesPassed );
  
  // samples passed per objects
  unsigned int numObjects = g_Query->getNumObjects();
  unsigned int *samplesPassed = new unsigned int[numObjects];
  g_Query->getSamplesPassedPerMesh( samplesPassed, numObjects);
  delete samplesPassed;
  
  // depth buffer
  ocull::DepthBuffer db( g_Query->getDepthBufferDesc() );
  g_Query->getDepthBuffer( &db );
}

