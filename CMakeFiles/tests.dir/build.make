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
CMAKE_SOURCE_DIR = /home/aluno/tmig

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aluno/tmig

# Include any dependencies generated for this target.
include CMakeFiles/tests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tests.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tests.dir/flags.make

CMakeFiles/tests.dir/tests/main.cpp.o: CMakeFiles/tests.dir/flags.make
CMakeFiles/tests.dir/tests/main.cpp.o: tests/main.cpp
CMakeFiles/tests.dir/tests/main.cpp.o: CMakeFiles/tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aluno/tmig/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tests.dir/tests/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tests.dir/tests/main.cpp.o -MF CMakeFiles/tests.dir/tests/main.cpp.o.d -o CMakeFiles/tests.dir/tests/main.cpp.o -c /home/aluno/tmig/tests/main.cpp

CMakeFiles/tests.dir/tests/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tests.dir/tests/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aluno/tmig/tests/main.cpp > CMakeFiles/tests.dir/tests/main.cpp.i

CMakeFiles/tests.dir/tests/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tests.dir/tests/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aluno/tmig/tests/main.cpp -o CMakeFiles/tests.dir/tests/main.cpp.s

# Object files for target tests
tests_OBJECTS = \
"CMakeFiles/tests.dir/tests/main.cpp.o"

# External object files for target tests
tests_EXTERNAL_OBJECTS =

tests/bin/tests: CMakeFiles/tests.dir/tests/main.cpp.o
tests/bin/tests: CMakeFiles/tests.dir/build.make
tests/bin/tests: lib/libtmig.so
tests/bin/tests: /usr/lib/x86_64-linux-gnu/libOpenGL.so
tests/bin/tests: /usr/lib/x86_64-linux-gnu/libGLX.so
tests/bin/tests: /usr/lib/x86_64-linux-gnu/libGLU.so
tests/bin/tests: /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
tests/bin/tests: CMakeFiles/tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aluno/tmig/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tests/bin/tests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tests.dir/build: tests/bin/tests
.PHONY : CMakeFiles/tests.dir/build

CMakeFiles/tests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tests.dir/clean

CMakeFiles/tests.dir/depend:
	cd /home/aluno/tmig && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aluno/tmig /home/aluno/tmig /home/aluno/tmig /home/aluno/tmig /home/aluno/tmig/CMakeFiles/tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tests.dir/depend

