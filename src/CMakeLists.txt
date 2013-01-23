set(ITKIOSCIFIO_SRC
itkSCIFIOImageIOFactory.cxx
itkSCIFIOImageIO.cxx
)

add_library(ITKIOSCIFIO ${ITKIOSCIFIO_SRC})
target_link_libraries(ITKIOSCIFIO  ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOSCIFIO)

# Download the SCIFIO Java libraries.
set( SCIFIOJars
  loci_tools.jar
  scifio.jar
  )
set( SCIFIOJars_SHA256
  3058f1d46da7e7f75f4d230459a7c1ca1ef293b2d1ffe883ff027ed0fcdf210c
  81b24966e6d08a453a42f1097e3dd3616335ef75cb9aa828edc112050577133f
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
