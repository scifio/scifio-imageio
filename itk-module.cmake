set(DOCUMENTATION "This module contains classes for reading and writing image
files with the Open Microscopy Environment (OME) Scientific Imaging Formats
Input and Output (SCIFIO) library.")

itk_module(SCIFIO
  ENABLE_SHARED
  DEPENDS
    ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
  EXCLUDE_FROM_DEFAULT
  DESCRIPTION
    "${DOCUMENTATION}"
)
