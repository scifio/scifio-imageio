itk_module_test()
set(ITKIOSCIFIOTests
itkSCIFIOImageIOTest.cxx
)

CreateTestDriver(ITKIOSCIFIO  "${ITKIOSCIFIO-Test_LIBRARIES}" "${ITKIOSCIFIOTests}")

#itk_add_test( NAME ITKSCIFIOImageIOTest
  #COMMAND ITKIOSCIFIOTestDriver
  #--compare DATA{Input/biorad.pic} ${ITK_TEST_OUTPUT_DIR}/biorad_new.pic
  #itkSCIFIOImageIOTest DATA{Input/biorad.pic} ${ITK_TEST_OUTPUT_DIR}/biorad_new.pic )
