cmake_minimum_required( VERSION 3.2 )
project( dipot )
set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_CXX_STANDARD 14 )

file( GLOB INCLUDES cformat/*.h driver.h )

set( LIBRARY_PATH $ENV{LD_LIBRARY_PATH} )
set( spot ${LIBRARY_PATH}/libspot.a )
set( bddx ${LIBRARY_PATH}/libbddx.a )

include_directories( "/usr/local/include/" )
include_directories( ${INCLUDE} )

include(bricks/support.cmake)
bricks_check_dirent()
set( BRICK_USED_LLVM_LIBS ${BRICK_LLVM_LIBS} )
include_directories( bricks )

add_definitions( -DBRICKS_HAVE_LLVM )

file( GLOB SRC format/*.cpp dipot.cpp )
add_executable(dipot ${SRC} ${INCLUDES} )
target_link_libraries( dipot spot bddx )

install( TARGETS dipot DESTINATION build )
