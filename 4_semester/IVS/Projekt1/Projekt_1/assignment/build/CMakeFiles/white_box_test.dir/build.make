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
include CMakeFiles/white_box_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/white_box_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/white_box_test.dir/flags.make

CMakeFiles/white_box_test.dir/white_box_tests.o: CMakeFiles/white_box_test.dir/flags.make
CMakeFiles/white_box_test.dir/white_box_tests.o: ../white_box_tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/white_box_test.dir/white_box_tests.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/white_box_test.dir/white_box_tests.o -c /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_tests.cpp

CMakeFiles/white_box_test.dir/white_box_tests.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/white_box_test.dir/white_box_tests.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_tests.cpp > CMakeFiles/white_box_test.dir/white_box_tests.i

CMakeFiles/white_box_test.dir/white_box_tests.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/white_box_test.dir/white_box_tests.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_tests.cpp -o CMakeFiles/white_box_test.dir/white_box_tests.s

CMakeFiles/white_box_test.dir/white_box_code.o: CMakeFiles/white_box_test.dir/flags.make
CMakeFiles/white_box_test.dir/white_box_code.o: ../white_box_code.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/white_box_test.dir/white_box_code.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/white_box_test.dir/white_box_code.o -c /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_code.cpp

CMakeFiles/white_box_test.dir/white_box_code.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/white_box_test.dir/white_box_code.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_code.cpp > CMakeFiles/white_box_test.dir/white_box_code.i

CMakeFiles/white_box_test.dir/white_box_code.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/white_box_test.dir/white_box_code.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/white_box_code.cpp -o CMakeFiles/white_box_test.dir/white_box_code.s

# Object files for target white_box_test
white_box_test_OBJECTS = \
"CMakeFiles/white_box_test.dir/white_box_tests.o" \
"CMakeFiles/white_box_test.dir/white_box_code.o"

# External object files for target white_box_test
white_box_test_EXTERNAL_OBJECTS =

white_box_test: CMakeFiles/white_box_test.dir/white_box_tests.o
white_box_test: CMakeFiles/white_box_test.dir/white_box_code.o
white_box_test: CMakeFiles/white_box_test.dir/build.make
white_box_test: lib/libgtest_main.a
white_box_test: lib/libgtest.a
white_box_test: CMakeFiles/white_box_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable white_box_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/white_box_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/white_box_test.dir/build: white_box_test

.PHONY : CMakeFiles/white_box_test.dir/build

CMakeFiles/white_box_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/white_box_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/white_box_test.dir/clean

CMakeFiles/white_box_test.dir/depend:
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build /home/punk4tt4ck/Desktop/VUT/4_semester/IVS/Projekt_1/assignment/build/CMakeFiles/white_box_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/white_box_test.dir/depend

