# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build

# Include any dependencies generated for this target.
include googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/depend.make

# Include the progress variables for this target.
include googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/progress.make

# Include the compile flags for this target's objects.
include googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make

googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.o: googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make
googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.o: googletest-master/googletest/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.o"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest_main.dir/src/gtest_main.o -c /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-master/googletest/src/gtest_main.cc

googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest_main.dir/src/gtest_main.i"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-master/googletest/src/gtest_main.cc > CMakeFiles/gtest_main.dir/src/gtest_main.i

googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest_main.dir/src/gtest_main.s"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-master/googletest/src/gtest_main.cc -o CMakeFiles/gtest_main.dir/src/gtest_main.s

# Object files for target gtest_main
gtest_main_OBJECTS = \
"CMakeFiles/gtest_main.dir/src/gtest_main.o"

# External object files for target gtest_main
gtest_main_EXTERNAL_OBJECTS =

lib/libgtest_main.a: googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.o
lib/libgtest_main.a: googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/build.make
lib/libgtest_main.a: googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgtest_main.a"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean_target.cmake
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/build: lib/libgtest_main.a

.PHONY : googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/build

googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/clean:
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean.cmake
.PHONY : googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/clean

googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/depend:
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-master/googletest /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : googletest-build/googlemock/gtest/CMakeFiles/gtest_main.dir/depend
