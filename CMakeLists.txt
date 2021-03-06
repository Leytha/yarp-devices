# Copyright: Universidad Carlos III de Madrid (C) 2013;
# Authors: Juan G. Victores & Raúl de Santos Rico
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ASROB_YARP_DEVICES)

# Add suffix for debug libraries.
if(MSVC)
    message(STATUS "Running on windows")
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Let the user specify a configuration (only single-config generators).
if(NOT CMAKE_CONFIGURATION_TYPES)
    # Possible values.
    set(_configurations Debug Release MinSizeRel RelWithDebInfo)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${_configurations})

    foreach(_conf ${_configurations})
        set(_conf_string "${_conf_string} ${_conf}")
    endforeach()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING
                 "Choose the type of build, options are:${_conf_string}")

    if(NOT CMAKE_BUILD_TYPE)
        # Encourage the user to specify build type.
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
    endif()
endif()

# Pick up our CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake
                              ${CMAKE_SOURCE_DIR}/cmake/find-modules)

# Hard dependencies.
find_package(YCM 0.8 REQUIRED)
# https://github.com/roboticslab-uc3m/questions-and-answers/issues/65
find_package(YARP 3.0 QUIET)
if(NOT YARP_FOUND)
    find_package(YARP 2.3.70 REQUIRED)
endif()
find_package(COLOR_DEBUG REQUIRED)

# Soft dependencies.
find_package(LibSerial 0.6 QUIET)
find_package(wiringPi QUIET)

# Always build YARP devices as MODULE libraries.
set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "Force dynamic plugins")

# Configure installation paths for YARP resources.
yarp_configure_external_installation(asrob-yarp-devices WITH_PLUGINS)

# Standard installation directories.
include(GNUInstallDirs)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Enabling coverage.
option(ENABLE_coverage "Choose if you want to enable coverage collection" OFF)

if(ENABLE_coverage)
    # List supported compilers.
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif()
endif()

# Create targets if specific requirements are satisfied.
include(CMakeDependentOption)

# Add main contents.
add_subdirectory(libraries)
add_subdirectory(share)
#add_subdirectory(tests)

# Store the package in the user registry.
export(PACKAGE ASROB_YARP_DEVICES)

# Create and install config files.
include(InstallBasicPackageFiles)

install_basic_package_files(ASROB_YARP_DEVICES
                            VERSION 0.1.0
                            COMPATIBILITY AnyNewerVersion
                            FIRST_TARGET RobotInterfaces
                            NO_SET_AND_CHECK_MACRO
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO
                            NAMESPACE ASROB::
                            NO_COMPATIBILITY_VARS)

# Configure and create uninstall target.
include(AddUninstallTarget)
