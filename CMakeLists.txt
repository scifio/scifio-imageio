cmake_minimum_required(VERSION 3.16.3)

project(SCIFIO
  VERSION 5.1.0       #Version should track with ITK
  LANGUAGES CXX)

set(SCIFIO_LIBRARIES SCIFIO)

if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
  include(ITKModuleExternal)
else()
  set(ITK_DIR ${CMAKE_BINARY_DIR})
  itk_module_impl()
endif()
