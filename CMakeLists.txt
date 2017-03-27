cmake_minimum_required( VERSION 3.2 )
project( dipot )
set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_CXX_STANDARD 14 )

foreach( D ui format )
     file( GLOB CPP_${D} ${D}/*.cpp )
     file( GLOB HPP_${D} ${D}/*.hpp )
     install( FILES ${HPP_${D}} DESTINATION include/divine/${D} )
endforeach()

set( DIPOT_INCLUDES ${dipot_SOURCE_DIR} )
include_directories( ${DIPOT_INCLUDES} ${CMAKE_BINARY_DIR} )

add_library( dipot-ui ${CPP_ui} )
add_library( dipot-format ${CPP_format} )

install( TARGETS dipot-ui dipot-format DESTINATION lib )
install( FILES ${HPP} DESTINATION include/dipot )

set( LIBRARY_PATH $ENV{LD_LIBRARY_PATH} )
set( spot ${LIBRARY_PATH}/libspot.a )
set( bddx ${LIBRARY_PATH}/libbddx.a )
include_directories( "/usr/local/include/" )

include(bricks/support.cmake)
bricks_check_dirent()
set( BRICK_USED_LLVM_LIBS ${BRICK_LLVM_LIBS} )
include_directories( bricks )

add_definitions( -DBRICKS_HAVE_LLVM )

file( GLOB SRC dipot.cpp )
add_executable(dipot ${SRC} ${INCLUDES} )
target_link_libraries( dipot spot bddx dipot-ui dipot-format )

install( TARGETS dipot DESTINATION build )
