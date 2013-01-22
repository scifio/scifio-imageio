itk_module_test()
set(ITKIOSCIFIOTests
itkSCIFIOImageIOTest.cxx
)

CreateTestDriver(ITKIOSCIFIO  "${ITKIOSCIFIO-Test_LIBRARIES}" "${ITKIOSCIFIOTests}")

itk_add_test( NAME ITKSCIFIOImageIOTest
  COMMAND ITKIOSCIFIOTestDriver
  --compare DATA{${ITK_DATA_ROOT}/Input/cthead1.tif} ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio.tif
  itkSCIFIOImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1.tif} ${ITK_TEST_OUTPUT_DIR}/cthead1_scifio.tif )
