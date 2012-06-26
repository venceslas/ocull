CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( ocull )

SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )
SET( OCULLDIR ${CMAKE_SOURCE_DIR}/../../lib )

# to change
set( THIRDPARTIES ${CMAKE_SOURCE_DIR}/../viewer/thirdparty )

FILE( GLOB_RECURSE Headers 
      ${OCULLDIR}/*.hpp )
      
FILE( GLOB_RECURSE Sources 
      ${OCULLDIR}/*.cpp )

#
# glew lib file set
#
file( GLOB glew 
      ${THIRDPARTIES}/GL/*.c 
      ${THIRDPARTIES}/GL/*.h )
add_library( GLEW ${glew} )

INCLUDE_DIRECTORIES( ${THIRDPARTIES} )


INCLUDE_DIRECTORIES( ${OCULLDIR} )

INCLUDE_DIRECTORIES( /usr/local/cuda/include )
INCLUDE( FindCUDA )
LINK_LIBRARIES( "-L/usr/lib/nvidia-current -lcuda" )

# removing a warning added with cuda 4.2
SET( CMAKE_CXX_FLAGS "-Wno-conversion-null")

SET( Libraries GL glut m )


CUDA_ADD_EXECUTABLE( ${CMAKE_PROJECT_NAME} 
                     main.cpp 
                     ${Sources} ${Headers} )

TARGET_LINK_LIBRARIES( ${CMAKE_PROJECT_NAME} ${Libraries} GLEW)

