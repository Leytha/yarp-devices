# Created by Juan G Victores 2013 @ UC3M
# Thanks to Paul Fitzpatrick for all the YARP and iCub SWIG stuff for inspiration

cmake_minimum_required(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
link_directories(${YARP_LIBRARY_DIRS})
link_libraries(${YARP_LIBRARIES})

find_package(ASROB_YARP_DEVICES REQUIRED)
include_directories(${ASROB_YARP_DEVICES_INCLUDE_DIRS})
link_directories(${ASROB_YARP_DEVICES_LINK_DIRS})
link_libraries(${ASROB_YARP_DEVICES_LIBRARIES})

# Work-around for missing paths to OpenCV libraries
find_package(OpenCV)
if (OpenCV_FOUND)
  link_directories(${OpenCV_LINK_DIRECTORIES} ${OPENCV_LINK_DIRECTORIES})
endif (OpenCV_FOUND)

#MESSAGE(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../src/libraries/ColorClientLib)

# for yarp.i
include_directories($ENV{YARP_ROOT}/bindings)

set(CREATE_PYTHON FALSE CACHE BOOL "Create Python interface")
set(CREATE_RUBY FALSE CACHE BOOL "Create Ruby interface")
set(CREATE_JAVA FALSE CACHE BOOL "Create Java interface")
SET(CREATE_CSHARP FALSE CACHE BOOL "Create CSharp interface")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
MESSAGE(STATUS "${SWIG_USE_FILE}")
SET (MIN_SWIG_VERSION "1.3.29")
IF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")
  MESSAGE( FATAL_ERROR "Please update your SWIG. Current version: ${SWIG_VERSION}. The minimum version needed: ${MIN_SWIG_VERSION}")
ENDIF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")

SET_SOURCE_FILES_PROPERTIES(asrob_yarp_devices.i PROPERTIES CPLUSPLUS ON)

if (CREATE_PYTHON)
  set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}")
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;asrob_yarp_devices;-threads")
  SET(CREATE_PYTHON_VERSION "" CACHE STRING "Control python version used, if you wish." )
  FIND_PACKAGE(PythonLibs ${CREATE_PYTHON_VERSION})
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
  LINK_LIBRARIES(${PYTHON_LIBRARY})
  
  ## in Windows it seems necessary to declare explictly the link_directory
  if (WIN32)
    get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
    link_directories(${PYTHON_DIR})
  endif()
  
  SWIG_ADD_MODULE(asrob_yarp_devices python asrob_yarp_devices.i)
  # uncomment if we want a debug version of the library
  #ADD_DEFINITIONS(-DPy_DEBUG)

  # installation path is determined reliably on most platforms using distutils
  FIND_PACKAGE(PythonInterp ${CREATE_PYTHON_VERSION})

  EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} 
    -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))" 
    OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
  INSTALL(FILES ${CMAKE_BINARY_DIR}/asrob_yarp_devices.py ${CMAKE_BINARY_DIR}/_asrob_yarp_devices.so 
    DESTINATION ${PYTHON_INSTDIR} )

  IF (WIN32)
    # Check if we have a target called _asrob_yarp_devices as SWIG_ADD_MODULE
    # will currently give. If not, we're probably in the distant
    # future, and we'd best not meddle.
    GET_TARGET_PROPERTY(asrob_yarp_devices_lib_location _asrob_yarp_devices LOCATION)
    IF (asrob_yarp_devices_lib_location)
      SET_TARGET_PROPERTIES(_asrob_yarp_devices PROPERTIES SUFFIX ".pyd")
    ENDIF ()    
  ENDIF (WIN32)
endif (CREATE_PYTHON)

if (CREATE_RUBY)
  find_package(Ruby REQUIRED)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;asrob_yarp_devices")
  include_directories(${RUBY_INCLUDE_PATH})
  swig_add_module(asrob_yarp_devices ruby asrob_yarp_devices.i)
  swig_link_libraries(asrob_yarp_devices ${RUBY_LIBRARY})
  set_target_properties(asrob_yarp_devices PROPERTIES PREFIX "")
endif (CREATE_RUBY)

if (CREATE_JAVA)
  OPTION(PREPARE_CLASS_FILES "Prepare class files" OFF)
  SET(ROBOT_DEVASTATION_ROBOTS_HELPERS LoadAsibot.java)
  MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/asrob_yarp_devices")
  SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/asrob_yarp_devices")
  SET(TARGET_DIR src)
  IF (NOT BINDINGS_SRC_DIR)
    SET(BINDINGS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ENDIF ()
  IF (NOT PREPARE_CLASS_FILES)
    SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/generated_src")
    SET(TARGET_DIR generated_src)
  ENDIF ()
  MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}")
  FOREACH(src ${ROBOT_DEVASTATION_ROBOTS_HELPERS})
    SET(target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/${src})
    CONFIGURE_FILE(${BINDINGS_SRC_DIR}/${src}
      ${target} IMMEDIATE)
    MESSAGE(STATUS "copied ${target}")
  ENDFOREACH(src)
  SET(CMAKE_SWIG_FLAGS "-package;asrob_yarp_devices;-Wall;-module;asrob_yarp_devices")
  FIND_PACKAGE(JNI REQUIRED)
  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
  SET(target_name jasrob_yarp_devices)
  SWIG_ADD_MODULE(${target_name} java asrob_yarp_devices.i)
  IF (PREPARE_CLASS_FILES)
    ADD_CUSTOM_COMMAND(TARGET ${target_name}
      POST_BUILD
      COMMAND javac -source 1.3 -target 1.3 "src/*.java" "asrob_yarp_devices/*.java"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF ()

  # issue on MINGW where C++ name-mangling prevents java finding methods
  IF (MINGW)
    MESSAGE(STATUS "untested MINGW patch - see CMakeLists.txt")
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES 
      LINK_FLAGS "--add-stdcall-alias")
  ENDIF (MINGW)
  ## the following is based on:
  ## http://www.cmake.org/pipermail/cmake/2007-September/016343.html
  IF(APPLE)
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES SUFFIX ".jnilib")
  ENDIF(APPLE)
endif (CREATE_JAVA)

IF (CREATE_CSHARP)
   SET(target_name asrob_yarp_devices)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;asrob_yarp_devices")
   SWIG_ADD_MODULE(asrob_yarp_devices csharp asrob_yarp_devices.i)
ENDIF (CREATE_CSHARP)
