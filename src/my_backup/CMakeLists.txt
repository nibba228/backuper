cmake_minimum_required(VERSION 3.22)

project(my_backup)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)


find_package(Boost 1.82.0 COMPONENTS
  filesystem
  program_options
  REQUIRED
)

add_subdirectory(backup)

add_executable(${PROJECT_NAME} my_backup.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/backup"
                           "${PROJECT_SOURCE_DIR}/../options"
                           "${PROJECT_SOURCE_DIR}/../util"
                           "${PROJECT_SOURCE_DIR}/../util/backup"
                           "${PROJECT_SOURCE_DIR}/../util/filesystem"
)

find_package(fmt)
target_link_libraries(my_backup
  Boost::filesystem
  Boost::program_options
  fmt::fmt
  options
  backup
  util
)