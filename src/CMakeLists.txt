cmake_minimum_required(VERSION 3.22)
project(backup_restore)

set(CMAKE_CXX_COMPIER "/usr/bin/clang++-18")

add_subdirectory(options)
add_subdirectory(util)

option(BACKUP "Build my_backup target")

if(BACKUP)
  add_subdirectory(my_backup)
else()
  add_subdirectory(my_restore)
endif()