# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/eric/Desktop/OpenCVdemo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eric/Desktop/OpenCVdemo/build

# Include any dependencies generated for this target.
include CMakeFiles/sift.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sift.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sift.dir/flags.make

CMakeFiles/sift.dir/FeaturePoint_SIFT.o: CMakeFiles/sift.dir/flags.make
CMakeFiles/sift.dir/FeaturePoint_SIFT.o: ../FeaturePoint_SIFT.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eric/Desktop/OpenCVdemo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sift.dir/FeaturePoint_SIFT.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sift.dir/FeaturePoint_SIFT.o -c /home/eric/Desktop/OpenCVdemo/FeaturePoint_SIFT.cpp

CMakeFiles/sift.dir/FeaturePoint_SIFT.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sift.dir/FeaturePoint_SIFT.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eric/Desktop/OpenCVdemo/FeaturePoint_SIFT.cpp > CMakeFiles/sift.dir/FeaturePoint_SIFT.i

CMakeFiles/sift.dir/FeaturePoint_SIFT.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sift.dir/FeaturePoint_SIFT.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eric/Desktop/OpenCVdemo/FeaturePoint_SIFT.cpp -o CMakeFiles/sift.dir/FeaturePoint_SIFT.s

CMakeFiles/sift.dir/FeaturePoint_SIFT.o.requires:

.PHONY : CMakeFiles/sift.dir/FeaturePoint_SIFT.o.requires

CMakeFiles/sift.dir/FeaturePoint_SIFT.o.provides: CMakeFiles/sift.dir/FeaturePoint_SIFT.o.requires
	$(MAKE) -f CMakeFiles/sift.dir/build.make CMakeFiles/sift.dir/FeaturePoint_SIFT.o.provides.build
.PHONY : CMakeFiles/sift.dir/FeaturePoint_SIFT.o.provides

CMakeFiles/sift.dir/FeaturePoint_SIFT.o.provides.build: CMakeFiles/sift.dir/FeaturePoint_SIFT.o


# Object files for target sift
sift_OBJECTS = \
"CMakeFiles/sift.dir/FeaturePoint_SIFT.o"

# External object files for target sift
sift_EXTERNAL_OBJECTS =

sift: CMakeFiles/sift.dir/FeaturePoint_SIFT.o
sift: CMakeFiles/sift.dir/build.make
sift: /opt/ros/kinetic/lib/libopencv_stitching3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_superres3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_videostab3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_aruco3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_bgsegm3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_bioinspired3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_ccalib3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_cvv3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_datasets3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_dpm3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_face3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_fuzzy3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_hdf3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_line_descriptor3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_optflow3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_plot3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_reg3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_saliency3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_stereo3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_structured_light3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_surface_matching3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_text3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_xfeatures2d3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_ximgproc3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_xobjdetect3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_xphoto3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_shape3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_video3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_viz3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_phase_unwrapping3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_rgbd3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_calib3d3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_features2d3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_flann3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_objdetect3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_ml3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_highgui3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_photo3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_videoio3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_imgcodecs3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_imgproc3.so.3.2.0
sift: /opt/ros/kinetic/lib/libopencv_core3.so.3.2.0
sift: CMakeFiles/sift.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/eric/Desktop/OpenCVdemo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable sift"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sift.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sift.dir/build: sift

.PHONY : CMakeFiles/sift.dir/build

CMakeFiles/sift.dir/requires: CMakeFiles/sift.dir/FeaturePoint_SIFT.o.requires

.PHONY : CMakeFiles/sift.dir/requires

CMakeFiles/sift.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sift.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sift.dir/clean

CMakeFiles/sift.dir/depend:
	cd /home/eric/Desktop/OpenCVdemo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/eric/Desktop/OpenCVdemo /home/eric/Desktop/OpenCVdemo /home/eric/Desktop/OpenCVdemo/build /home/eric/Desktop/OpenCVdemo/build /home/eric/Desktop/OpenCVdemo/build/CMakeFiles/sift.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sift.dir/depend
