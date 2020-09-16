cmake_minimum_required(VERSION 3.12)
project(SilverBullet C CXX)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Relese")
endif()

set(MY_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.10.x
  GIT_SHALLOW
)
set(googletest_BUILD_TESTS OFF)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR})
endif()

add_subdirectory(base)

add_subdirectory(util)

add_subdirectory(net)

add_executable(SilverBullet)
target_sources(SilverBullet PRIVATE "main.cc")
target_link_libraries(SilverBullet PRIVATE base util net)
link_libraries(pthread)

target_compile_definitions(SilverBullet PRIVATE std_cxx_14)  
target_compile_options(SilverBullet PRIVATE
  -g
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 -rdynamic
 )
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")