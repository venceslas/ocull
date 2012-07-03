///
///   This sample file was just for the lib to compile (to test minor bug)
///

#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <ocull/ocull.hpp>


int main( int argc, char *argv[])
{
  glutInit( &argc, argv);
  glutCreateWindow( NULL );  
  glewInit();

  const char *shaderPath = "../viewer/data/shader/pipeShader.cubin";
  //ocull::Context *context = NULL;
  
  //ocull::Context::Create( shaderPath );

/*  
  ocull::Query query( 800, 600);
    
  ocull::Camera camera;  
  camera.setFrustum( ocull::Frustum() );
  camera.setViewMatrix( ocull::Matrix4x4() );
  
  query.setCamera( camera );  
  //query.setDepthBuffer();
  
  
  /// -------------------------------------
  
  query.resize( 1280, 720);
  
  context->begin( &query );
    //context->uploadMesh();
  context->end();
  
  ocull::Query::Result result = query.getResults();
  
  /// -------------------------------------  
*/
  
  //ocull::Context::Release( context );
  
  return EXIT_SUCCESS;
}
