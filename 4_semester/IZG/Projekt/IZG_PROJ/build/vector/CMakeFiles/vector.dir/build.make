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
CMAKE_SOURCE_DIR = /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build

# Include any dependencies generated for this target.
include vector/CMakeFiles/vector.dir/depend.make

# Include the progress variables for this target.
include vector/CMakeFiles/vector.dir/progress.make

# Include the compile flags for this target's objects.
include vector/CMakeFiles/vector.dir/flags.make

vector/CMakeFiles/vector.dir/src/vector/vector.c.o: vector/CMakeFiles/vector.dir/flags.make
vector/CMakeFiles/vector.dir/src/vector/vector.c.o: ../vector/src/vector/vector.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object vector/CMakeFiles/vector.dir/src/vector/vector.c.o"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/vector.dir/src/vector/vector.c.o   -c /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/vector/src/vector/vector.c

vector/CMakeFiles/vector.dir/src/vector/vector.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vector.dir/src/vector/vector.c.i"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/vector/src/vector/vector.c > CMakeFiles/vector.dir/src/vector/vector.c.i

vector/CMakeFiles/vector.dir/src/vector/vector.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vector.dir/src/vector/vector.c.s"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/vector/src/vector/vector.c -o CMakeFiles/vector.dir/src/vector/vector.c.s

# Object files for target vector
vector_OBJECTS = \
"CMakeFiles/vector.dir/src/vector/vector.c.o"

# External object files for target vector
vector_EXTERNAL_OBJECTS =

vector/libvector.a: vector/CMakeFiles/vector.dir/src/vector/vector.c.o
vector/libvector.a: vector/CMakeFiles/vector.dir/build.make
vector/libvector.a: vector/CMakeFiles/vector.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libvector.a"
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && $(CMAKE_COMMAND) -P CMakeFiles/vector.dir/cmake_clean_target.cmake
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
vector/CMakeFiles/vector.dir/build: vector/libvector.a

.PHONY : vector/CMakeFiles/vector.dir/build

vector/CMakeFiles/vector.dir/clean:
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector && $(CMAKE_COMMAND) -P CMakeFiles/vector.dir/cmake_clean.cmake
.PHONY : vector/CMakeFiles/vector.dir/clean

vector/CMakeFiles/vector.dir/depend:
	cd /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/vector /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector /home/punk4tt4ck/Desktop/VUT/4_semester/IZG/Projekt/IZG_PROJ/build/vector/CMakeFiles/vector.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : vector/CMakeFiles/vector.dir/depend

