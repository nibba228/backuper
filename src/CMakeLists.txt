cmake_minimum_required(VERSION 3.22)
project(backup_restore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(options)
add_subdirectory(util)

find_package(Boost 1.82.0 COMPONENTS
  filesystem
  program_options
  REQUIRED
)

find_package(fmt)

add_subdirectory(my_backup)
add_subdirectory(my_restore)