# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = "/Applications/CMake 2.8-3.app/Contents/bin/cmake"

# The command to remove a file.
RM = "/Applications/CMake 2.8-3.app/Contents/bin/cmake" -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "/Applications/CMake 2.8-3.app/Contents/bin/ccmake"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/awilcox/Wilcox Tech/eScape"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/awilcox/Wilcox Tech/eScape/build/iPhone"

# Include any dependencies generated for this target.
include CMakeFiles/b64.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/b64.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/b64.dir/flags.make

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o: CMakeFiles/b64.dir/flags.make
CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o: ../../contrib/libb64/src/cdecode.c
	$(CMAKE_COMMAND) -E cmake_progress_report "/Users/awilcox/Wilcox Tech/eScape/build/iPhone/CMakeFiles" $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o   -c "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cdecode.c"

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.i"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -E "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cdecode.c" > CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.i

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.s"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -S "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cdecode.c" -o CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.s

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.requires:
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.requires

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.provides: CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.requires
	$(MAKE) -f CMakeFiles/b64.dir/build.make CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.provides.build
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.provides

CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.provides.build: CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.provides.build

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o: CMakeFiles/b64.dir/flags.make
CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o: ../../contrib/libb64/src/cencode.c
	$(CMAKE_COMMAND) -E cmake_progress_report "/Users/awilcox/Wilcox Tech/eScape/build/iPhone/CMakeFiles" $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o   -c "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cencode.c"

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.i"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -E "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cencode.c" > CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.i

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.s"
	/usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -S "/Users/awilcox/Wilcox Tech/eScape/contrib/libb64/src/cencode.c" -o CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.s

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.requires:
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.requires

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.provides: CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.requires
	$(MAKE) -f CMakeFiles/b64.dir/build.make CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.provides.build
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.provides

CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.provides.build: CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o
.PHONY : CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.provides.build

# Object files for target b64
b64_OBJECTS = \
"CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o" \
"CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o"

# External object files for target b64
b64_EXTERNAL_OBJECTS =

libb64.a: CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o
libb64.a: CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o
libb64.a: CMakeFiles/b64.dir/build.make
libb64.a: CMakeFiles/b64.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C static library libb64.a"
	$(CMAKE_COMMAND) -P CMakeFiles/b64.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/b64.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/b64.dir/build: libb64.a
.PHONY : CMakeFiles/b64.dir/build

CMakeFiles/b64.dir/requires: CMakeFiles/b64.dir/contrib/libb64/src/cdecode.c.o.requires
CMakeFiles/b64.dir/requires: CMakeFiles/b64.dir/contrib/libb64/src/cencode.c.o.requires
.PHONY : CMakeFiles/b64.dir/requires

CMakeFiles/b64.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/b64.dir/cmake_clean.cmake
.PHONY : CMakeFiles/b64.dir/clean

CMakeFiles/b64.dir/depend:
	cd "/Users/awilcox/Wilcox Tech/eScape/build/iPhone" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/awilcox/Wilcox Tech/eScape" "/Users/awilcox/Wilcox Tech/eScape" "/Users/awilcox/Wilcox Tech/eScape/build/iPhone" "/Users/awilcox/Wilcox Tech/eScape/build/iPhone" "/Users/awilcox/Wilcox Tech/eScape/build/iPhone/CMakeFiles/b64.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/b64.dir/depend

