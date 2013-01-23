set(ITKIOSCIFIO_SRC
itkSCIFIOImageIOFactory.cxx
itkSCIFIOImageIO.cxx
)

add_library(ITKIOSCIFIO ${ITKIOSCIFIO_SRC})
target_link_libraries(ITKIOSCIFIO  ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOSCIFIO)

# Download the SCIFIO Java libraries.
#
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
set( downloadDirectory Jars )
set( SCIFIOJarsDownloadPaths )
foreach( jar ${SCIFIOJars} )
  set( SCIFIOJarsDownloadPaths "${SCIFIOJarsDownloadPaths}
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${downloadDirectory}/${jar}" )
endforeach()
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

install( DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${downloadDirectory}/
  DESTINATION ${${ITKIOSCIFIO}_INSTALL_LIBRARY_DIR}/${downloadDirectory}
  COMPONENT RuntimeLibraries
  FILES_MATCHING PATTERN "*.jar"
  )
