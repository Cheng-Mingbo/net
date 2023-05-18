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
CMAKE_SOURCE_DIR = /tmp/libnet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/libnet/cmake-build-debug

# Include any dependencies generated for this target.
include tests/CMakeFiles/test_Server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/test_Server.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test_Server.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test_Server.dir/flags.make

tests/CMakeFiles/test_Server.dir/test_Server.cpp.o: tests/CMakeFiles/test_Server.dir/flags.make
tests/CMakeFiles/test_Server.dir/test_Server.cpp.o: ../tests/test_Server.cpp
tests/CMakeFiles/test_Server.dir/test_Server.cpp.o: tests/CMakeFiles/test_Server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/libnet/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/test_Server.dir/test_Server.cpp.o"
	cd /tmp/libnet/cmake-build-debug/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/test_Server.dir/test_Server.cpp.o -MF CMakeFiles/test_Server.dir/test_Server.cpp.o.d -o CMakeFiles/test_Server.dir/test_Server.cpp.o -c /tmp/libnet/tests/test_Server.cpp

tests/CMakeFiles/test_Server.dir/test_Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_Server.dir/test_Server.cpp.i"
	cd /tmp/libnet/cmake-build-debug/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/libnet/tests/test_Server.cpp > CMakeFiles/test_Server.dir/test_Server.cpp.i

tests/CMakeFiles/test_Server.dir/test_Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_Server.dir/test_Server.cpp.s"
	cd /tmp/libnet/cmake-build-debug/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/libnet/tests/test_Server.cpp -o CMakeFiles/test_Server.dir/test_Server.cpp.s

# Object files for target test_Server
test_Server_OBJECTS = \
"CMakeFiles/test_Server.dir/test_Server.cpp.o"

# External object files for target test_Server
test_Server_EXTERNAL_OBJECTS =

tests/test_Server: tests/CMakeFiles/test_Server.dir/test_Server.cpp.o
tests/test_Server: tests/CMakeFiles/test_Server.dir/build.make
tests/test_Server: src/network/libnetwork.a
tests/test_Server: src/util/libutil.a
tests/test_Server: third_party/spdlog/libspdlogd.a
tests/test_Server: tests/CMakeFiles/test_Server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/libnet/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_Server"
	cd /tmp/libnet/cmake-build-debug/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_Server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test_Server.dir/build: tests/test_Server
.PHONY : tests/CMakeFiles/test_Server.dir/build

tests/CMakeFiles/test_Server.dir/clean:
	cd /tmp/libnet/cmake-build-debug/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_Server.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test_Server.dir/clean

tests/CMakeFiles/test_Server.dir/depend:
	cd /tmp/libnet/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/libnet /tmp/libnet/tests /tmp/libnet/cmake-build-debug /tmp/libnet/cmake-build-debug/tests /tmp/libnet/cmake-build-debug/tests/CMakeFiles/test_Server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test_Server.dir/depend

