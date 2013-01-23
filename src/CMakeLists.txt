# NB: For now, we use the loci_tools.jar bundle, because it is convenient,
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
set( SCIFIOJars
  loci_tools.jar
  )
set( SCIFIOJars_SHA256
  3058f1d46da7e7f75f4d230459a7c1ca1ef293b2d1ffe883ff027ed0fcdf210c
  )
set( downloadDirectory jars )
set( JAR_BUILD_TREE_LOCATION 
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${downloadDirectory}/
  )
set( JAR_INSTALL_TREE_LOCATION
  ${CMAKE_INSTALL_PREFIX}/${${ITKIOSCIFIO}_INSTALL_LIBRARY_DIR}/jre
  )

# Java Runtime Environment
set( jreTarballDownloadDirectory "${CMAKE_CURRENT_BINARY_DIR}" )
set( JRE_BUILD_TREE_LOCATION 
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${downloadDirectory}/
  )
set( JRE_INSTALL_TREE_LOCATION
  ${CMAKE_INSTALL_PREFIX}/${${ITKIOSCIFIO}_INSTALL_LIBRARY_DIR}/jre
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
set(ITKIOSCIFIO_SRC
  itkSCIFIOImageIOFactory.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/itkSCIFIOImageIO.cxx
  )

add_library(ITKIOSCIFIO ${ITKIOSCIFIO_SRC})
target_link_libraries(ITKIOSCIFIO  ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOSCIFIO)

# Download the SCIFIO Java libraries.
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/DownloadSCIFIO.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DownloadSCIFIO.cmake
  @ONLY
  )
add_custom_command(TARGET ITKIOSCIFIO
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

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/DownloadJRE.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DownloadJRE.cmake
  @ONLY
  )
add_custom_command(TARGET ITKIOSCIFIO
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/DownloadJRE.cmake
  WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  COMMENT "Downloading JRE libraries..."
  )
install( DIRECTORY ${JRE_BUILD_TREE_LOCATION}
  DESTINATION ${JRE_INSTALL_TREE_LOCATION}
  COMPONENT RuntimeLibraries
  )