set(ITKIOSCIFIO_SRC
itkSCIFIOImageIOFactory.cxx
itkSCIFIOImageIO.cxx
)

add_library(ITKIOSCIFIO ${ITKIOSCIFIO_SRC})
target_link_libraries(ITKIOSCIFIO  ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIOSCIFIO)
