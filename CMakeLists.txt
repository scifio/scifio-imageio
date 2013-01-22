###
# #%L
# SCIFIO ImageIO plugin for the Insight Toolkit.
# %%
# Copyright (C) 2010 - 2012 Insight Software Consortium, and Open Microscopy
# Environment:
#   - Board of Regents of the University of Wisconsin-Madison
#   - Glencoe Software, Inc.
#   - University of Dundee
# %%
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing official
# policies, either expressed or implied, of any organization.
# 
# ----------------------------------------------------------------
# Adapted from the Slicer3 project: http://www.slicer.org/
# http://viewvc.slicer.org/viewcvs.cgi/trunk/Libs/MGHImageIO/
# 
# See slicer-license.txt for Slicer3's licensing information.
# 
# For more information about the ITK Plugin IO mechanism, see:
# http://www.itk.org/Wiki/Plugin_IO_mechanisms
# #L%
###

project(SCIFIOImageIO)

cmake_minimum_required(VERSION 2.8)

# --------------------------------------------------------------------------
# Initial setup

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
include_directories(${include_dirs})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/itkSCIFIOImageIOConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/itkSCIFIOImageIOConfigure.h
)

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

if(NOT DEFINED BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "Build with shared libraries." ON)
endif(NOT DEFINED BUILD_SHARED_LIBS)

option(SCIFIO_WIN_BUILD_DEBUG "Build Debug version? If false, builds Release. Windows only." ON)
if(${SCIFIO_WIN_BUILD_DEBUG} STREQUAL "ON")
  if(NOT DEFINED SCIFIO_WIN_BUILD_VALUE)
    set(SCIFIO_WIN_BUILD_VALUE \\Debug)
  endif(NOT DEFINED SCIFIO_WIN_BUILD_VALUE)
elseif(${SCIFIO_WIN_BUILD_DEBUG} STREQUAL "OFF")
  if(NOT DEFINED SCIFIO_WIN_BUILD_VALUE)
    set(SCIFIO_WIN_BUILD_VALUE \\Release)
  endif(NOT DEFINED SCIFIO_WIN_BUILD_VALUE)
endif(${SCIFIO_WIN_BUILD_DEBUG} STREQUAL "ON")

# --------------------------------------------------------------------------
# Find ITK

find_package(ITK REQUIRED)
include("${ITK_USE_FILE}")

# --------------------------------------------------------------------------
# Check ITK version

# ITK 3.20.0 or newer is required.
# For ITK 3.x, ITK_USE_REVIEW must be turned ON.

if(ITK_VERSION_MAJOR LESS "3")
  message(FATAL_ERROR "The SCIFIO ImageIO plugin requires ITK 3.20.0 or newer. You are using ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}.")
endif(ITK_VERSION_MAJOR LESS "3")

if(ITK_VERSION_MAJOR EQUAL "3")
  if(ITK_VERSION_MINOR LESS "20")
    message(FATAL_ERROR "The SCIFIO ImageIO plugin requires ITK 3.20.0 or newer. You are using ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}.")
  endif(ITK_VERSION_MINOR LESS "20")
  if(NOT ITK_USE_REVIEW)
    message(FATAL_ERROR "For ITK 3.x, ITK_USE_REVIEW must be turned ON.")
  endif(NOT ITK_USE_REVIEW)
  set(SCIFIO_LINK_REVIEW ON)
endif(ITK_VERSION_MAJOR EQUAL "3")

if(ITK_VERSION_MAJOR GREATER "3")
  set(SCIFIO_LINK_REVIEW OFF)
endif(ITK_VERSION_MAJOR GREATER "3")

# --------------------------------------------------------------------------
# Find Java native library

find_package(Java REQUIRED)

# --------------------------------------------------------------------------
# Sources

set(SCIFIO_SRCS
  itkSCIFIOImageIO.cxx
  itkSCIFIOImageIOFactory.cxx
)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name SCIFIOIO)
add_library(${lib_name}
  ${SCIFIO_SRCS}
)

if(SCIFIO_LINK_REVIEW)
  # Review libraries required
  target_link_libraries(${lib_name}
    ${ITK_LIBRARIES}
    ITKIOReview
  )
else(SCIFIO_LINK_REVIEW)
  target_link_libraries(${lib_name}
    ${ITK_LIBRARIES}
  )
endif(SCIFIO_LINK_REVIEW)

# --------------------------------------------------------------------------
# Shared library that when placed in ITK_AUTOLOAD_PATH, will add
# SCIFIOImageIO as an ImageIOFactory.  Need to have separate shared
# library for each new format. Note that the plugin library is placed
# in a special directory to speed up the searching for ImageIO
# factories (which improves the speed at which plugins run).

set(SCIFIO_LIB_DIR "${CMAKE_BINARY_DIR}/lib")

add_library(SCIFIOIOPlugin MODULE
  itkSCIFIOIOPlugin.cxx
)
set_target_properties(SCIFIOIOPlugin
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
  "${SCIFIO_LIB_DIR}"
  LIBRARY_OUTPUT_DIRECTORY
  "${SCIFIO_LIB_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY
  "${SCIFIO_LIB_DIR}"
)
target_link_libraries(SCIFIOIOPlugin
  ${lib_name}
)

# --------------------------------------------------------------------------
# Add some tests

add_executable(itkSCIFIOImageIOTest itkSCIFIOImageIOTest.cxx)
target_link_libraries(itkSCIFIOImageIOTest SCIFIOIO)

add_executable(itkRGBSCIFIOImageIOTest itkRGBSCIFIOImageIOTest.cxx)
target_link_libraries(itkRGBSCIFIOImageIOTest SCIFIOIO)

add_executable(itkSCIFIOImageInfo itkSCIFIOImageInfo.cxx)
target_link_libraries(itkSCIFIOImageInfo SCIFIOIO)

add_executable(itkVectorImageSCIFIOImageIOTest
  itkVectorImageSCIFIOImageIOTest.cxx
)
target_link_libraries(itkVectorImageSCIFIOImageIOTest SCIFIOIO)

add_executable(perf perf.cxx)
target_link_libraries(perf SCIFIOIO)

# --------------------------------------------------------------------------
# Copy targets into distribution folder (dist/scifio-imageio)

set(SCIFIO_DIST_DIR "dist/scifio-imageio")

if(APPLE)
  set(SCIFIO_LIB_PREFIX "lib")
  set(SCIFIO_SUFFIX ".dylib")
  set(SCIFIO_BUILD_TYPE "")
elseif(WIN32 OR WIN64)
  set(SCIFIO_LIB_PREFIX "")
  set(SCIFIO_BUILD_TYPE ${SCIFIO_WIN_BUILD_VALUE})
  set(SCIFIO_SUFFIX ".dll")
else(APPLE) # assume UNIX
  set(SCIFIO_LIB_PREFIX "lib")
  set(SCIFIO_SUFFIX ".so")
  set(SCIFIO_BUILD_TYPE "")
endif(APPLE)

if(APPLE)
  add_custom_command(
    TARGET SCIFIOIOPlugin
    DEPENDS "${SCIFIO_LIB_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
      "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin${SCIFIO_SUFFIX}"
    VERBATIM
  )

  add_custom_command(
    TARGET SCIFIOIOPlugin
    DEPENDS "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
      "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
      "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
    VERBATIM
  )
  install(FILES
    "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin.so"
    DESTINATION "."
  )
endif(APPLE)

add_custom_command(
  TARGET SCIFIOIOPlugin
  DEPENDS "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin${SCIFIO_SUFFIX}"
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "${SCIFIO_LIB_DIR}/${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin${SCIFIO_SUFFIX}"
    "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin${SCIFIO_SUFFIX}"
  VERBATIM
)
install(FILES
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIOPlugin${SCIFIO_SUFFIX}"
  DESTINATION "."
)

add_custom_command(
  TARGET SCIFIOIOPlugin
  DEPENDS "${CMAKE_BINARY_DIR}${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIO${SCIFIO_SUFFIX}"
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_BINARY_DIR}${SCIFIO_BUILD_TYPE}/${SCIFIO_LIB_PREFIX}SCIFIOIO${SCIFIO_SUFFIX}"
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIO${SCIFIO_SUFFIX}"
  VERBATIM
)
install(FILES
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/${SCIFIO_LIB_PREFIX}SCIFIOIO${SCIFIO_SUFFIX}"
  DESTINATION "."
)

# --------------------------------------------------------------------------
# Define location of loci_tools.jar file. It is assumed that loci_tools.jar
# has been generated using "ant tools" from the SCIFIO base folder.

set(SCIFIO_ARTIFACT_DIR "${PROJECT_SOURCE_DIR}/../../../artifacts")
set(SCIFIO_LOCI_TOOLS_JAR "${SCIFIO_ARTIFACT_DIR}/loci_tools.jar")

# --------------------------------------------------------------------------
# Copy loci_tools.jar to the distribution folder (dist/scifio-imageio).

add_custom_command(
  TARGET SCIFIOIOPlugin
  DEPENDS "${SCIFIO_LOCI_TOOLS_JAR}"
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${SCIFIO_LOCI_TOOLS_JAR}"
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/loci_tools.jar"
  VERBATIM
)
install(FILES
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/loci_tools.jar"
  DESTINATION "."
)

# --------------------------------------------------------------------------
# Generate a list of extensions supported by SCIFIO, and copy the
# resultant SCIFIOSuffixes.txt to the distribution folder
# (dist/scifio-imageio).

add_custom_command(
  TARGET SCIFIOIOPlugin
  DEPENDS "${SCIFIO_LOCI_TOOLS_JAR}"
  POST_BUILD
  COMMAND "${JAVA_RUNTIME}" -cp "${SCIFIO_LOCI_TOOLS_JAR}"
  "loci.formats.tools.SCIFIOExtensionPrinter"
  VERBATIM
)

add_custom_command(
  TARGET SCIFIOIOPlugin
  DEPENDS "${CMAKE_BINARY_DIR}/SCIFIOSuffixes.txt"
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "${CMAKE_BINARY_DIR}/SCIFIOSuffixes.txt"
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/SCIFIOSuffixes.txt"
  VERBATIM
)
install(FILES
  "${CMAKE_BINARY_DIR}/${SCIFIO_DIST_DIR}/SCIFIOSuffixes.txt"
  DESTINATION "."
)

# --------------------------------------------------------------------------
# Package distribution folder (dist/scifio-imageio) into .zip and .tar.gz
# archives, which can be generated using the "make package" command.

set(CPACK_PACKAGE_FILE_NAME "scifio-imageio")
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)
