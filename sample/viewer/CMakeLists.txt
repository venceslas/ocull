cmake_minimum_required (VERSION 2.6)

project( ocull_viewer )

set( THIRDPARTIES ${CMAKE_SOURCE_DIR}/thirdparty )

#
# drone lib file set
#
file( GLOB_RECURSE drone_lib_files 
      ${THIRDPARTIES}/drone/*.c 
      ${THIRDPARTIES}/drone/*.h )

#
# drone_scene lib file set
#
file( GLOB_RECURSE drone_scene_lib_files 
      ${THIRDPARTIES}/drone_scene/*.cpp 
      ${THIRDPARTIES}/drone_scene/*.hpp )

add_definitions( -DDRN_DBG_LVL=0 )


#
# glew lib file set
#
file( GLOB glew 
      ${THIRDPARTIES}/GL/*.c 
      ${THIRDPARTIES}/GL/*.h )
add_library( GLEW ${glew} )


#
# glsw lib file set
#
file( GLOB glsw 
      ${THIRDPARTIES}/glsw/*.c 
      ${THIRDPARTIES}/glsw/*.h )
add_library( GLSW ${glsw} )

#
# gltext lib
#
add_subdirectory( ${THIRDPARTIES}/gltext )


#
# SFML lib
#
include( FindSFML )


file( GLOB_RECURSE Headers 
      src/*.hpp)
      
file( GLOB_RECURSE Sources 
      src/*.cpp )


include_directories( ${THIRDPARTIES} )
include_directories( src )

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR} )

#
# viewer
#
add_executable(viewer 
               ${drone_lib_files} ${drone_scene_lib_files}
               ${Sources} ${Headers})

target_link_libraries(viewer 
                      sfml-window ${GLTEXT_LIBRARIES} GLU GL GLEW GLSW )

