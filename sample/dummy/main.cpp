///
///   This sample file was just for the lib to compile (to test minor code error)
///

#include <cstdio>
#include <cstdlib>

#include <ocull/ocull.hpp> // rewrite (tmp)


int main( int argc, char *argv[])
{
  ocull::Context *context = ocull::Context::Create( "pipeShader.cubin" );
  
  ocull::Query query( 1280, 720);
  
  
  ocull::Camera camera;
  
  camera.setFrustum( ocull::Frustum() );
  camera.setViewMatrix( ocull::Matrix4x4() );
  
  query.setCamera( camera );
  
  //query.setDepthBuffer();
  
  context->begin( &query );
    //context->uploadMesh();
  context->end();
  
  ocull::Query::Result result = query.getResults();
  
  
  
  ocull::Context::Release( context );
  
  
  return EXIT_SUCCESS;
}
