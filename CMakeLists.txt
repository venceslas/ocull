CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( ocull )

SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )

# TODO: create locale CMakeLists.txt
FILE( GLOB_RECURSE Headers ${CMAKE_SOURCE_DIR}/lib/*.hpp )
FILE( GLOB_RECURSE Sources ${CMAKE_SOURCE_DIR}/lib/*.cpp )

INCLUDE_DIRECTORIES( /usr/local/cuda/include )

# TODO: more specific include directory
INCLUDE_DIRECTORIES( lib )

INCLUDE( FindCUDA )

SET( Libraries GL glut m)
LINK_LIBRARIES( "-L/usr/lib/nvidia-current -lcuda" )

# Static libraries to build
FILE( GLOB GLEW ${CMAKE_SOURCE_DIR}/thirdparty/GL/*.c extern/GL/*.h )
ADD_LIBRARY( glew ${GLEW} )
INCLUDE_DIRECTORIES( thirdparty )

# removing a warning added with cuda 4.2
set( CMAKE_CXX_FLAGS "-Wno-conversion-null")

CUDA_ADD_EXECUTABLE( ${CMAKE_PROJECT_NAME} ${Sources} ${Headers} )

TARGET_LINK_LIBRARIES( ${CMAKE_PROJECT_NAME} ${Libraries} glew)

