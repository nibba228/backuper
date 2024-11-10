add_subdirectory(restore)

add_executable(my_restore my_restore.cpp)
target_include_directories(my_restore PUBLIC "."
                          "./../options"
                          "./../util"
                          "./../util/backup"
                          "./../util/filesystem"
                          "./restore"
)

target_link_libraries(my_restore
  Boost::filesystem
  Boost::program_options
  fmt::fmt
  options
  restore
  util
)

# add_subdirectory(restore)

# cmake_minimum_required(VERSION 3.22)

# project(my_restore)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# find_package(Boost 1.82.0 COMPONENTS
#   filesystem
#   program_options
#   REQUIRED
# )

# add_executable(${PROJECT_NAME} my_restore.cpp)


# target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}"
#                            "${PROJECT_SOURCE_DIR}/../options"
#                            "${PROJECT_SOURCE_DIR}/../util"
#                            "${PROJECT_SOURCE_DIR}/../util/backup"
#                            "${PROJECT_SOURCE_DIR}/../util/filesystem"
#                            "{$PROJECT_SOURCE_DIR}/restore"
# )

# find_package(fmt)
# target_link_libraries(my_restore
#   Boost::filesystem
#   Boost::program_options
#   fmt::fmt
#   options
#   restore
#   util
# )