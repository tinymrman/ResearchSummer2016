# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.8.9)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()

# This project is designed to be built outside the Insight source tree.
project(Minkowski)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_definitions("-std=c++14")
  set(CMAKE_CXX_FLAGS "-std=c++14")
endif()

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(Minkowski
    itkMinkowski.h
    itkMinkowski.cxx
    itkMinkowskiCalculator.h
    )

target_link_libraries(Minkowski ${ITK_LIBRARIES})

