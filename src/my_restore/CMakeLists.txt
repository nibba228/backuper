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