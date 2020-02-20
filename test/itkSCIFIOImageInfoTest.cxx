/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"
#include "itkImageIOBase.h"
#include "itkSCIFIOImageIO.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define METADATA_NOT_FOUND "No value for this key."

#define assertEquals(name, expected, actual)                                                                           \
  {                                                                                                                    \
    if (expected != actual)                                                                                            \
    {                                                                                                                  \
      std::cerr << "[ERROR] " << name                                                                                  \
                << " does not match: "                                                                                 \
                   "expected="                                                                                         \
                << expected                                                                                            \
                << "; "                                                                                                \
                   "actual="                                                                                           \
                << actual << std::endl;                                                                                \
      return EXIT_FAILURE;                                                                                             \
    }                                                                                                                  \
  }

namespace
{
/*
 * Splits a string into tokens using the given delimiter.
 *
 * Thanks to SO #236129 for this solution:
 * http://stackoverflow.com/a/236803
 */
std::vector<std::string> &
split(const std::string & s, char delim, std::vector<std::string> & elems)
{
  std::stringstream ss(s);
  std::string       item;
  while (std::getline(ss, item, delim))
  {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string>
split(const std::string & s, char delim)
{
  std::vector<std::string> elems;
  return split(s, delim, elems);
}
} // namespace


int
itkSCIFIOImageInfoTest(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " argument\n";
    return EXIT_FAILURE;
  }

  // NB: We pass all arguments as a single argument, to work around our
  // lack of understanding of CMake. I could not immediately wrangle CMake
  // into passing the arguments without them being quoted as a single one,
  // so instead we simply split the arguments here, which suffices.
  std::vector<std::string> argList = split(std::string(argv[1]), ' ');

  if (argList.size() != 5)
  {
    std::cerr << "Argument not of the form: sizeX sizeY sizeZ sizeT sizeC\n";
    return EXIT_FAILURE;
  }
  const std::string sX = argList[0];
  const std::string sY = argList[1];
  const std::string sZ = argList[2];
  const std::string sT = argList[3];
  const std::string sC = argList[4];

  // Create a fake file on disk, for use with testing the SCIFIO ImageIO
  // reader. SCIFIO does not actually care whether the file exists.
  const std::string id = "scifioImageInfo"
                         "&sizeX=" +
                         sX + "&sizeY=" + sY + "&sizeZ=" + sZ + "&sizeT=" + sT + "&sizeC=" + sC + ".fake";

  int sizeX = atoi(sX.c_str());
  int sizeY = atoi(sY.c_str());
  int sizeZ = atoi(sZ.c_str());
  int sizeT = atoi(sT.c_str());
  int sizeC = atoi(sC.c_str());

  using PixelType = unsigned char;
  constexpr unsigned int Dimension = 5;

  using ImageType = itk::Image<PixelType, Dimension>;
  using ReaderType = itk::ImageFileReader<ImageType>;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();

  io->DebugOn();

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(io);

  reader->SetFileName(id);
  reader->Update();

  ImageType::Pointer img = reader->GetOutput();

  const int actualSizeX = img->GetLargestPossibleRegion().GetSize()[0];
  const int actualSizeY = img->GetLargestPossibleRegion().GetSize()[1];
  const int actualSizeZ = img->GetLargestPossibleRegion().GetSize()[2];
  const int actualSizeT = img->GetLargestPossibleRegion().GetSize()[3];
  const int actualSizeC = img->GetLargestPossibleRegion().GetSize()[4];

  assertEquals("sizeX", sizeX, actualSizeX);
  assertEquals("sizeY", sizeY, actualSizeY);
  assertEquals("sizeZ", sizeZ, actualSizeZ);
  assertEquals("sizeT", sizeT, actualSizeT);
  assertEquals("sizeC", sizeC, actualSizeC);

  // TODO: Pass more parameters (e.g., pixelType) to this test class,
  // and assert that the itk::Image structure matches those values.

  // Dump the metadata dictionary
  std::cout << std::endl;
  std::cout << "--== Metadata from dictionary ==--" << std::endl;
  itk::MetaDataDictionary  imgMetaDictionary = img->GetMetaDataDictionary();
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  for (std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin(); itKey != imgMetaKeys.end(); ++itKey)
  {
    std::string tmp;
    itk::ExposeMetaData<std::string>(imgMetaDictionary, *itKey, tmp);
    std::cout << "\t" << *itKey << " ---> " << tmp << std::endl;
  }
  std::cout << std::endl;

  // Dump the metadata naturally contained within ImageIOBase
  const itk::ImageIOBase * imageIO = reader->GetImageIO();
  itk::ImageIORegion       region = imageIO->GetIORegion();
  int                      regionDim = region.GetImageDimension();
  std::cout << "--== Metadata from ImageIOBase ==--" << std::endl;
  for (int i = 0; i < regionDim; i++)
  {
    std::cout << "\tDimension " << i + 1 << " Size: " << region.GetSize(i) << std::endl;
  }
  for (int i = 0; i < regionDim; i++)
  {
    if (region.GetSize(i) > 1)
    {
      std::cout << "\tSpacing " << i + 1 << ": " << imageIO->GetSpacing(i) << std::endl;
    }
  }
  std::cout << "\tByte Order: " << imageIO->GetByteOrderAsString(imageIO->GetByteOrder()) << std::endl;
  std::cout << "\tPixel Stride: " << imageIO->GetPixelStride() << std::endl;
  std::cout << "\tPixel Type: " << imageIO->GetPixelTypeAsString(imageIO->GetPixelType()) << std::endl;
  std::cout << "\tImage Size (in pixels): " << imageIO->GetImageSizeInPixels() << std::endl;
  std::cout << "\tPixel Type: " << imageIO->GetComponentTypeAsString(imageIO->GetComponentType()) << std::endl;
  std::cout << "\tRGB Channel Count: " << imageIO->GetNumberOfComponents() << std::endl;
  std::cout << "\tNumber of Dimensions: " << imageIO->GetNumberOfDimensions() << std::endl;
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
