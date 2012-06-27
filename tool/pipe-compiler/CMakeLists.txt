CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( cubinPipeCompiler )

SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )
SET( CUDARASTER ${CMAKE_SOURCE_DIR}/../../lib/ocull/rasterizer )

FILE( GLOB_RECURSE Headers 
      ${CUDARASTER}/*.hpp )
      
FILE( GLOB_RECURSE Sources 
      ${CUDARASTER}/*.cpp )

#
# glew lib file set
#
SET( THIRDPARTIES ${CMAKE_SOURCE_DIR}/../../sample/viewer/thirdparty )
file( GLOB glew 
      ${THIRDPARTIES}/GL/*.c 
      ${THIRDPARTIES}/GL/*.h )
add_library( GLEW ${glew} )

INCLUDE( FindCUDA )
LINK_LIBRARIES( "-L/usr/lib/nvidia-current -lcuda" )


INCLUDE_DIRECTORIES( ${THIRDPARTIES} )
INCLUDE_DIRECTORIES( ${CUDARASTER} )
INCLUDE_DIRECTORIES( /usr/local/cuda/include )


# removing a warning added with cuda 4.2
SET( CMAKE_CXX_FLAGS "-Wno-conversion-null")

SET( Libraries GL glut)


CUDA_ADD_EXECUTABLE( ${CMAKE_PROJECT_NAME} 
                     main.cpp 
                     ${Sources} ${Headers} )

TARGET_LINK_LIBRARIES( ${CMAKE_PROJECT_NAME} 
                       ${Libraries} 
                       GLEW )

