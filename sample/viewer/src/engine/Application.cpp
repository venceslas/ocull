///
///
///


#include "Application.hpp"

#include <cstdlib>
#include <cstdio>

#include <GL/glew.h>

#include <cuda.h>



namespace engine {


void Application::init(int argc, char *argv[])
{
  // Order matters
  
  _initContext( argc, argv);
  _initOpenGL( argc, argv);
  _initCUDA( argc, argv);
  _initData( argc, argv);
}


void Application::_initContext( int argc, char *argv[])
{
  fprintf( stderr, "Error: no graphic context has been created.\n");
  exit( EXIT_FAILURE );
}

void Application::_initOpenGL( int argc, char *argv[])
{
  /// Init OpenGL Extension Wrangler
  
  glewExperimental = GL_TRUE;
  GLenum result = glewInit();
  
  if (GLEW_OK != result)
  {
    fprintf( stderr, "Error: %s\n", glewGetErrorString(result));
    exit( EXIT_FAILURE );
  }
  fprintf( stderr, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
    
  
  /// Init OpenGL wrapper
  
  //OpenGL::Init();
  
  // sometimes the error handler is not initialized
  glGetError();
}

void Application::_initCUDA( int argc, char *argv[])
{
  if (CUDA_SUCCESS != cuInit(0))
  {
    fprintf( stderr, "Error: CUDA initialization failed.\n");
    exit( EXIT_FAILURE );
  }
}

void Application::_initData( int argc, char *argv[])
{
  // empty
}

} //namespace engine

