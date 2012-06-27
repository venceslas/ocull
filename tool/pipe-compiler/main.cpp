///
///   \file main.cpp
///
///   This is a standalone compiler to create cache version (cubin file)
///   of the rasterizer pipeline shaders.
///


#include <cstdlib>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "CudaRaster.hpp"
#include "framework/gpu/CudaCompiler.hpp"
#include "framework/base/Defs.hpp"


int main( int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "usage:" << std::endl
              << argv[0] << " <cudaraster path> <vertex shader path>"
              << std::endl;
    exit( EXIT_FAILURE );
  }
  
  std::string crPath( argv[1] );
  std::string vsPath( argv[2] );
  
  
  /** The CudaRaster framework use OpenGL extension, which need glew,
   *  which must be initialized after the window context. So in order
   *  to use the CR framework's compiler without modifying all useless
   *  OpenGL calls, we have to specify a dummy window.
   */  
  glutInit( &argc, argv);
  glutCreateWindow( NULL );  
  glewInit();
  
  /// Compiles the pipeline shaders  
  FW::CudaCompiler cudaCompiler;
  cudaCompiler.include( crPath );
  cudaCompiler.include( crPath + "/framework" );  
  cudaCompiler.setSourceFile( vsPath );  
  
  // Compile CUDA code
  FW::U32 renderModeFlags = 0u;
  renderModeFlags |= FW::RenderModeFlag_EnableDepth;
  renderModeFlags |= FW::RenderModeFlag_EnableLerp;
  renderModeFlags |= FW::RenderModeFlag_EnableQuads;
  
  cudaCompiler.clearDefines();
  cudaCompiler.define( "SAMPLES_LOG2", 0);//
  cudaCompiler.define( "RENDER_MODE_FLAGS", renderModeFlags);
  cudaCompiler.define( "BLEND_SHADER", "BlendReplace");
  
  // ProfilingMode : Default, Counters or Timers
  cudaCompiler.define( "CR_PROFILING_MODE", ProfilingMode_Default); //
  

  FW::CudaModule *cudaModule = cudaCompiler.compile();
  
  if (NULL == cudaModule) 
  {
    std::cerr << "failed to compile." << std::endl;
    exit( EXIT_FAILURE );
  }
  
  delete cudaModule; 

  return EXIT_SUCCESS;
}
