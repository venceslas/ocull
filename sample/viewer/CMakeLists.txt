cmake_minimum_required (VERSION 2.6)

project(viewer)


#
# drone lib file set
#
file( GLOB_RECURSE drone_lib_files 
      ${CMAKE_SOURCE_DIR}/thirdparty/drone/*.c 
      ${CMAKE_SOURCE_DIR}/thirdparty/drone/*.h )

#
# drone_scene lib file set
#
file( GLOB_RECURSE drone_scene_lib_files 
      ${CMAKE_SOURCE_DIR}/thirdparty/drone_scene/*.cpp 
      ${CMAKE_SOURCE_DIR}/thirdparty/drone_scene/*.hpp )
add_definitions( -DDRN_DBG_LVL=0 )

#
# glew lib file set
#
file( GLOB glew 
      ${CMAKE_SOURCE_DIR}/thirdparty/GL/*.c 
      ${CMAKE_SOURCE_DIR}/thirdparty/GL/*.h )
add_library( GLEW ${glew} )

#
# glsw lib file set
#
file( GLOB glsw 
      ${CMAKE_SOURCE_DIR}/thirdparty/glsw/*.c 
      ${CMAKE_SOURCE_DIR}/thirdparty/glsw/*.h )
add_library( GLSW ${glsw} )


###############################################################################

#
# ocull lib file set
#
file( GLOB_RECURSE ocull
      ${CMAKE_SOURCE_DIR}/../../lib/*.cpp
      ${CMAKE_SOURCE_DIR}/../../lib/*.cpp
      ${CMAKE_SOURCE_DIR}/../../lib/*.hpp )


# CUDA
include( FindCUDA )
include_directories( /usr/local/cuda/include )
link_libraries( "-L/usr/lib/nvidia-current -lcuda" )

# TODO: compile the library apart

###############################################################################


file( GLOB_RECURSE Headers 
      src/*.hpp)
      
file( GLOB_RECURSE Sources 
      src/*.cpp )


include_directories( thirdparty )
include_directories( src )
include_directories( ../../lib ) #

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )

#
# viewer
#
cuda_add_executable( viewer 
                     ${drone_lib_files} ${drone_scene_lib_files}
                     ${Sources} ${Headers}
                     ${ocull})

target_link_libraries(viewer glut GLU GL GLEW GLSW)

