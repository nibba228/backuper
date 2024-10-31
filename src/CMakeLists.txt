cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++-18")

project(my_backup)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.82.0 COMPONENTS
  filesystem
  program_options
  system
  REQUIRED
)

add_subdirectory(options)

add_executable(${PROJECT_NAME} my_backup.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/options"
)

find_package(fmt)
target_link_libraries(my_backup
  Boost::filesystem
  Boost::program_options
  Boost::system
  fmt::fmt
  options
)