# NB: For now, we use the bioformats_package.jar bundle, because it is convenient,
# and provides support for all supported file formats, including those
# provided by the GPLed Bio-Formats library. ITK communicates with Java via
# pipes, which is generally accepted as "indirect" enough to avoid legal
# complications with linking to GPLed code.
#
# However, in the future, if this is deemed unacceptable, we need only
# change the list of JARs below to include only BSD-licensed SCIFIO JARs
# (including all their dependencies, of course). The downside is that
# Bio-Formats-specific file formats, including all proprietary microscopy
# formats, will no longer work "out of the box" -- the user will instead
# need to manually add the GPLed Bio-Formats JARs into the mix first.
#
# JRE is not downloaded for OS X. The cleanest way to have a JRE installed
# is to let the OS X take care of it. Once the tests are run or if you call
# scifio-imageio in your code, the OS will :
# - ask you to install JAVA through the system update on OS 10.7 and 10.8
# - ask you to download JAVA from Java's website and install it by yourself
# for OS 10.9.
#
set( SCIFIOJars
  https://downloads.openmicroscopy.org/bio-formats/5.0.4/artifacts/bioformats_package.jar
  https://maven.imagej.net/service/local/repositories/releases/content/io/scif/scifio-itk-bridge/1.2.0/scifio-itk-bridge-1.2.0.jar
  )
set( SCIFIOJarNames
  bioformats_package.jar
  scifio-itk-bridge.jar
  )
set( jarsDownloadDirectory jars )
set( JAR_BUILD_TREE_LOCATION
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${jarsDownloadDirectory}/
  )
set( JAR_INSTALL_TREE_LOCATION
  ${SCIFIO_INSTALL_LIBRARY_DIR}/${jarsDownloadDirectory}/
  )

# Java Runtime Environment
set( jreTarballDownloadDirectory "${CMAKE_CURRENT_BINARY_DIR}" )
set( JRE_BUILD_TREE_LOCATION
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/jre
  )
set( JRE_INSTALL_TREE_LOCATION
  ${SCIFIO_INSTALL_LIBRARY_DIR}
  )
set( jreTarballMD5 "" )
if( WIN32 )
  if( "${CMAKE_SIZEOF_VOID_P}" EQUAL 8 )
    # win64
    set( jreTarballMD5 "5be3a27fc46997785ee70e0e0f140135" )
  else()
    # win32
    set( jreTarballMD5 "a0f445bbe2f44e52a56f99e0e9d33c1a" )
  endif()
elseif( "${CMAKE_SYSTEM_NAME}" MATCHES "Linux" )
  if( "${CMAKE_SIZEOF_VOID_P}" EQUAL 8 )
    # linux-amd64
    set( jreTarballMD5 "4c75412942f7d36c5e8b8fb38ba975bc" )
  else()
    # linux
    set( jreTarballMD5 "777bd36ea81ca1b732ef8115a347fd66" )
  endif()
endif()


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/itkSCIFIOImageIO.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/itkSCIFIOImageIO.cxx
  )
set(SCIFIO_SRC
  itkSCIFIOImageIOFactory.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/itkSCIFIOImageIO.cxx
  )

add_library(SCIFIO ${ITK_LIBRARY_BUILD_TYPE} ${SCIFIO_SRC})
target_link_libraries(SCIFIO  ${ITKIOImageBase_LIBRARIES})
itk_module_target(SCIFIO)

# Download the SCIFIO Java libraries.
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/DownloadSCIFIO.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DownloadSCIFIO.cmake
  @ONLY
  )
add_custom_command(TARGET SCIFIO
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/DownloadSCIFIO.cmake
  WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  COMMENT "Downloading SCIFIO libraries..."
  )
install( DIRECTORY ${JAR_BUILD_TREE_LOCATION}
  DESTINATION ${JAR_INSTALL_TREE_LOCATION}
  COMPONENT RuntimeLibraries
  FILES_MATCHING PATTERN "*.jar"
  )

# Download JRE except for OS X
IF (NOT APPLE)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/DownloadJRE.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DownloadJRE.cmake
    @ONLY
    )
  add_custom_command(TARGET SCIFIO
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/DownloadJRE.cmake
    WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMENT "Downloading JRE libraries..."
    )
  install( DIRECTORY ${JRE_BUILD_TREE_LOCATION}
    DESTINATION ${JRE_INSTALL_TREE_LOCATION}
    COMPONENT RuntimeLibraries
    USE_SOURCE_PERMISSIONS
    )
endif()
