# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/code/video/Camkit

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/code/video/Camkit/build

# Include any dependencies generated for this target.
include demo/CMakeFiles/cktool.dir/depend.make

# Include the progress variables for this target.
include demo/CMakeFiles/cktool.dir/progress.make

# Include the compile flags for this target's objects.
include demo/CMakeFiles/cktool.dir/flags.make

demo/CMakeFiles/cktool.dir/cktool.c.o: demo/CMakeFiles/cktool.dir/flags.make
demo/CMakeFiles/cktool.dir/cktool.c.o: ../demo/cktool.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/code/video/Camkit/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object demo/CMakeFiles/cktool.dir/cktool.c.o"
	cd /home/pi/code/video/Camkit/build/demo && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/cktool.dir/cktool.c.o   -c /home/pi/code/video/Camkit/demo/cktool.c

demo/CMakeFiles/cktool.dir/cktool.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cktool.dir/cktool.c.i"
	cd /home/pi/code/video/Camkit/build/demo && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/pi/code/video/Camkit/demo/cktool.c > CMakeFiles/cktool.dir/cktool.c.i

demo/CMakeFiles/cktool.dir/cktool.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cktool.dir/cktool.c.s"
	cd /home/pi/code/video/Camkit/build/demo && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/pi/code/video/Camkit/demo/cktool.c -o CMakeFiles/cktool.dir/cktool.c.s

demo/CMakeFiles/cktool.dir/cktool.c.o.requires:
.PHONY : demo/CMakeFiles/cktool.dir/cktool.c.o.requires

demo/CMakeFiles/cktool.dir/cktool.c.o.provides: demo/CMakeFiles/cktool.dir/cktool.c.o.requires
	$(MAKE) -f demo/CMakeFiles/cktool.dir/build.make demo/CMakeFiles/cktool.dir/cktool.c.o.provides.build
.PHONY : demo/CMakeFiles/cktool.dir/cktool.c.o.provides

demo/CMakeFiles/cktool.dir/cktool.c.o.provides.build: demo/CMakeFiles/cktool.dir/cktool.c.o

# Object files for target cktool
cktool_OBJECTS = \
"CMakeFiles/cktool.dir/cktool.c.o"

# External object files for target cktool
cktool_EXTERNAL_OBJECTS =

demo/cktool: demo/CMakeFiles/cktool.dir/cktool.c.o
demo/cktool: demo/CMakeFiles/cktool.dir/build.make
demo/cktool: src/libcamkit.so
demo/cktool: /usr/lib/arm-linux-gnueabihf/libavutil.so
demo/cktool: /usr/lib/arm-linux-gnueabihf/libavcodec.so
demo/cktool: /usr/lib/arm-linux-gnueabihf/libswscale.so
demo/cktool: demo/CMakeFiles/cktool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable cktool"
	cd /home/pi/code/video/Camkit/build/demo && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cktool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
demo/CMakeFiles/cktool.dir/build: demo/cktool
.PHONY : demo/CMakeFiles/cktool.dir/build

demo/CMakeFiles/cktool.dir/requires: demo/CMakeFiles/cktool.dir/cktool.c.o.requires
.PHONY : demo/CMakeFiles/cktool.dir/requires

demo/CMakeFiles/cktool.dir/clean:
	cd /home/pi/code/video/Camkit/build/demo && $(CMAKE_COMMAND) -P CMakeFiles/cktool.dir/cmake_clean.cmake
.PHONY : demo/CMakeFiles/cktool.dir/clean

demo/CMakeFiles/cktool.dir/depend:
	cd /home/pi/code/video/Camkit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/code/video/Camkit /home/pi/code/video/Camkit/demo /home/pi/code/video/Camkit/build /home/pi/code/video/Camkit/build/demo /home/pi/code/video/Camkit/build/demo/CMakeFiles/cktool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : demo/CMakeFiles/cktool.dir/depend

