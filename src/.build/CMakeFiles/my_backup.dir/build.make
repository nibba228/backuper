# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/erestor/mipt/cyberprotect/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/erestor/mipt/cyberprotect/src/.build

# Include any dependencies generated for this target.
include CMakeFiles/my_backup.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/my_backup.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/my_backup.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/my_backup.dir/flags.make

CMakeFiles/my_backup.dir/my_backup.cpp.o: CMakeFiles/my_backup.dir/flags.make
CMakeFiles/my_backup.dir/my_backup.cpp.o: ../my_backup.cpp
CMakeFiles/my_backup.dir/my_backup.cpp.o: CMakeFiles/my_backup.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erestor/mipt/cyberprotect/src/.build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/my_backup.dir/my_backup.cpp.o"
	/usr/bin/clang++-18 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/my_backup.dir/my_backup.cpp.o -MF CMakeFiles/my_backup.dir/my_backup.cpp.o.d -o CMakeFiles/my_backup.dir/my_backup.cpp.o -c /home/erestor/mipt/cyberprotect/src/my_backup.cpp

CMakeFiles/my_backup.dir/my_backup.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/my_backup.dir/my_backup.cpp.i"
	/usr/bin/clang++-18 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/erestor/mipt/cyberprotect/src/my_backup.cpp > CMakeFiles/my_backup.dir/my_backup.cpp.i

CMakeFiles/my_backup.dir/my_backup.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/my_backup.dir/my_backup.cpp.s"
	/usr/bin/clang++-18 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/erestor/mipt/cyberprotect/src/my_backup.cpp -o CMakeFiles/my_backup.dir/my_backup.cpp.s

# Object files for target my_backup
my_backup_OBJECTS = \
"CMakeFiles/my_backup.dir/my_backup.cpp.o"

# External object files for target my_backup
my_backup_EXTERNAL_OBJECTS =

my_backup: CMakeFiles/my_backup.dir/my_backup.cpp.o
my_backup: CMakeFiles/my_backup.dir/build.make
my_backup: /usr/local/lib/libboost_program_options.so.1.82.0
my_backup: /usr/local/lib/libboost_filesystem.so.1.82.0
my_backup: /usr/lib/x86_64-linux-gnu/libfmt.so.8.1.1
my_backup: options/liboptions.a
my_backup: /usr/local/lib/libboost_atomic.so.1.82.0
my_backup: CMakeFiles/my_backup.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/erestor/mipt/cyberprotect/src/.build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable my_backup"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_backup.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/my_backup.dir/build: my_backup
.PHONY : CMakeFiles/my_backup.dir/build

CMakeFiles/my_backup.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my_backup.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my_backup.dir/clean

CMakeFiles/my_backup.dir/depend:
	cd /home/erestor/mipt/cyberprotect/src/.build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erestor/mipt/cyberprotect/src /home/erestor/mipt/cyberprotect/src /home/erestor/mipt/cyberprotect/src/.build /home/erestor/mipt/cyberprotect/src/.build /home/erestor/mipt/cyberprotect/src/.build/CMakeFiles/my_backup.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/my_backup.dir/depend
