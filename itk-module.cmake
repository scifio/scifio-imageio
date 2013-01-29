set(DOCUMENTATION "This module contains classes for reading and writing image
files with the Open Microscopy Envionment (OME) Scientific Imaging Formats
Input and Output (SCIFIO) library.")

itk_module(ITKIOSCIFIO
  DEPENDS
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
