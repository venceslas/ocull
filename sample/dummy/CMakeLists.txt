CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( ocull )

SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )


SET( LIBDIR ${CMAKE_SOURCE_DIR}/../../lib/ )
SET( OCULLDIR ${LIBDIR}/ocull ) # TODO

FILE( GLOB_RECURSE Headers 
      ${OCULLDIR}/*.hpp )
      
FILE( GLOB_RECURSE Sources 
      ${OCULLDIR}/*.cpp )

INCLUDE_DIRECTORIES( ${LIBDIR} )


#
# glew lib file set
#
# to change
set( THIRDPARTIES ${CMAKE_SOURCE_DIR}/../viewer/thirdparty )

file( GLOB glew 
      ${THIRDPARTIES}/GL/*.c 
      ${THIRDPARTIES}/GL/*.h )
add_library( GLEW ${glew} )

INCLUDE_DIRECTORIES( ${THIRDPARTIES} )


#
# CUDA
#
INCLUDE( FindCUDA )
LINK_LIBRARIES( "-L/usr/lib/nvidia-current -lcuda" )
INCLUDE_DIRECTORIES( /usr/local/cuda/include )

# removing a warning added with cuda 4.2
SET( CMAKE_CXX_FLAGS "-Wno-conversion-null")



SET( Libraries GL glut m )

CUDA_ADD_EXECUTABLE( ${CMAKE_PROJECT_NAME} 
                     main.cpp 
                     ${Sources} ${Headers} )

TARGET_LINK_LIBRARIES( ${CMAKE_PROJECT_NAME} ${Libraries} GLEW)

