itk_module_test()
set(ITKIOSCIFIOTests
itkRGBSCIFIOImageIOTest.cxx
itkSCIFIOImageIOTest.cxx
itkSCIFIOImageInfoTest.cxx
itkVectorImageSCIFIOImageIOTest.cxx
)

CreateTestDriver(ITKIOSCIFIO  "${ITKIOSCIFIO-Test_LIBRARIES}" "${ITKIOSCIFIOTests}")

# -- Test parsing of various (simulated) image data --

# Test input using itk::Image
set(scifioImageInfoTests
"37 41 1 1 1"
"23 90 1 2 3"
"1 1 11 7 5"
)
list(LENGTH scifioImageInfoTests scifioImageInfoTestCount)
math(EXPR scifioImageInfoTestRange "${scifioImageInfoTestCount} - 1")
foreach( i RANGE ${scifioImageInfoTestRange} )
  list(GET scifioImageInfoTests ${i} scifioImageInfoTest)
  itk_add_test( NAME ITKSCIFIOImageInfoTest${i}
    COMMAND ITKIOSCIFIOTestDriver
    itkSCIFIOImageInfoTest ${scifioImageInfoTest} )
endforeach()

# -- Test conversion of real image data --

# Test I/O using itk::Image
itk_add_test( NAME ITKSCIFIOImageIOTest
  COMMAND ITKIOSCIFIOTestDriver
  --compare DATA{${ITK_DATA_ROOT}/Input/cthead1.tif}
                 ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio.tif
  itkSCIFIOImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1.tif}
                            ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio.tif )

# Test I/O using itk::RGBPixel
itk_add_test( NAME ITKRGBSCIFIOImageIOTest
  COMMAND ITKIOSCIFIOTestDriver
  --compare DATA{${ITK_DATA_ROOT}/Input/image_color.bmp}
                 ${ITK_TEST_OUTPUT_DIR}/image_color_scifio.png
  itkRGBSCIFIOImageIOTest DATA{${ITK_DATA_ROOT}/Input/image_color.bmp}
                               ${ITK_TEST_OUTPUT_DIR}/image_color_scifio.png )

# Test I/O using itk::VectorImage
itk_add_test( NAME ITKVectorImageSCIFIOImageIOTest
  COMMAND ITKIOSCIFIOTestDriver
  --compare DATA{${ITK_DATA_ROOT}/Input/cthead1.tif}
                 ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio_vector.tif
  itkVectorImageSCIFIOImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1.tif}
                                       ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio_vector.tif )
