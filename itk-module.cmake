set(DOCUMENTATION "This module contains classes for reading and writing image
files with the OME SCIFIO library.  To use the module, a Java runtime
must be installed on the system.")

itk_module(ITKIOSCIFIO
  DEPENDS
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
