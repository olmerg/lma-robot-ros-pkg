#
# The find_package macro for Orocos-RTT works best with
# cmake >= 2.6.3
#
cmake_minimum_required(VERSION 2.6.3)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
#
# This creates a standard cmake project. You may extend this file with
# any cmake macro you see fit.
#
project(lma_acker_rtt)

#
# Do setup in case of ros package, If ROS_ROOT is set, it is
# recommended to use RTT/OCL through the ros packages.
#
set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

rosbuild_find_ros_package(rtt )

find_package(OROCOS-RTT HINTS ${rtt_PACKAGE_PATH}/../install )

# Defines the orocos_* cmake macros. See that file for additional
# documentation.
include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake)

# Set the CMAKE_PREFIX_PATH in case you're not using Orocos through ROS
# for helping these find commands find RTT.
find_package(OROCOS-RTT REQUIRED ${RTT_HINTS})



#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#
orocos_component(${PROJECT_NAME} src/AckermanRobot.cpp)
#
# Generates and installs our package. Must be the last statement such
# that it can pick up all above settings.
#
orocos_generate_package()



