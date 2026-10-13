cmake_minimum_required(VERSION 3.9)
project(mirte_screen)

# Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++20)
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

include(CheckIncludeFileCXX)

set(MIRTE_EXTRA_LIBS "")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# for iwyu, use iwyu_tool.py -p . -j 8
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40
            --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_SHA1
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
# if GIT_SHA1 is empty, read it from the git_sha1.txt file, which is generated
# in the CI build

# Find catkin macros and libraries if COMPONENTS list like find_package(catkin
# REQUIRED COMPONENTS xyz) is used, also find other catkin packages

find_package(Boost REQUIRED COMPONENTS system thread)

add_subdirectory(libs/tmx-cpp EXCLUDE_FROM_ALL)
include_directories(
  include/ libs/tmx-cpp/include/ libs/tmx-cpp/libs/async_serial/include/ ${Boost_INCLUDE_DIRS})


add_executable(
  ${PROJECT_NAME}_node
  src/main.cpp)


# Specify libraries to link a library or executable target against
target_link_libraries(
  ${PROJECT_NAME}_node
  Boost::system
  Boost::thread
  tmx_cpp
  ${MIRTE_EXTRA_LIBS})
