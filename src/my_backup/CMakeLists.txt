add_subdirectory(backup)

add_executable(my_backup my_backup.cpp)
target_include_directories(my_backup PUBLIC "."
                            "./backup"
                            "./../options"
                            "./../util"
                            "./../util/backup"
                            "./../util/filesystem"
)

target_link_libraries(my_backup
  Boost::filesystem
  Boost::program_options
  fmt::fmt
  options
  backup
  util
)