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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"
#include "itkImageIOBase.h"
#include "itkSCIFIOImageIO.h"

#define METADATA_NOT_FOUND "No value for this key."

namespace
{
  /*
   * Splits a string into tokens using the given delimiter.
   *
   * Thanks to SO #236129 for this solution:
   * http://stackoverflow.com/a/236803
   */
  std::vector<std::string> &split( const std::string &s, char delim,
                                   std::vector<std::string> &elems )
  {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }

  std::vector<std::string> split( const std::string &s, char delim )
  {
    std::vector<std::string> elems;
    return split(s, delim, elems);
  }
}

int itkSCIFIOImageInfoTest( int argc, char * argv[] )
{
  if( argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " argument\n";
    return EXIT_FAILURE;
  }

  // NB: We pass all arguments as a single argument, to work around our
  // lack of understanding of CMake. I could not immediately wrangle CMake
  // into passing the arguments without them being quoted as a single one,
  // so instead we simply split the arguments here, which suffices.
  std::vector<std::string> argList = split(std::string(argv[1]), ' ');

  if( argList.size() != 3 )
  {
    std::cerr << "Argument not of the form: sizeZ sizeC sizeT\n";
    return EXIT_FAILURE;
  }
  std::string sZ = argList[0];
  std::string sC = argList[1];
  std::string sT = argList[2];

  // Create a fake file on disk, for use with testing the SCIFIO ImageIO
  // reader. SCIFIO does not actually care whether the file exists.
  std::string id = "scifioImageInfo"
                   "&sizeZ=" + sZ +
                   "&sizeC=" + sC +
                   "&sizeT=" + sT +
                   ".fake";

  int sizeZ = atoi(sZ.c_str());
  int sizeC = atoi(sC.c_str());
  int sizeT = atoi(sT.c_str());

  std::cout << "--------- itkSCIFIOImageInfoTest ----------" << std::endl;

  typedef unsigned char     PixelType;
  const unsigned int        Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();

  io->DebugOn();

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(io);

  reader->SetFileName(inputFile);
  reader->Update();

  ImageType::Pointer img = reader->GetOutput();

  std::string metaString (METADATA_NOT_FOUND);
  itk::MetaDataDictionary imgMetaDictionary = img->GetMetaDataDictionary();
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin();

  // Iterate through the keys and print their paired values
  std::cout << "Metadata Key ---> Value pairs, from dictionary:" << std::endl;
  for(; itKey != imgMetaKeys.end(); ++itKey)
  {
    std::string tmp;
    itk::ExposeMetaData<std::string>( imgMetaDictionary, *itKey, tmp );
    std::cout << *itKey << " ---> " << tmp << std::endl;
    //std::cout << "Metadata: " << notes1 << std::endl;
    metaString = METADATA_NOT_FOUND;
  }

  // Print out the metadata naturally contained within itkImageIOBase
  itk::ImageIORegion region = reader->GetImageIO()->GetIORegion();
  int regionDim = region.GetImageDimension();

  std::cout << "Metadata Key ---> Value pairs, from ImageIOBase:" << std::endl;

  for(int i = 0; i < regionDim; i++)
  {
    std::cout << "Dimension " << i + 1 << " Size: " << region.GetSize(i) << std::endl;
  }
  for(int i = 0; i < regionDim; i++)
  {
    std::cout << "Spacing " << i + 1 << ": " << reader->GetImageIO()->GetSpacing(i) << std::endl;
  }
  std::cout << "Byte Order: " << reader->GetImageIO()->GetByteOrderAsString(reader->GetImageIO()->GetByteOrder()) << std::endl;
  std::cout << "Pixel Stride: " << reader->GetImageIO()->GetPixelStride() << std::endl;
  std::cout << "Pixel Type: " << reader->GetImageIO()->GetPixelTypeAsString(reader->GetImageIO()->GetPixelType()) << std::endl;
  std::cout << "Image Size (in pixels): " << reader->GetImageIO()->GetImageSizeInPixels() << std::endl;
  std::cout << "Pixel Type: " << reader->GetImageIO()->GetComponentTypeAsString(reader->GetImageIO()->GetComponentType()) << std::endl;
  std::cout << "RGB Channel Count: " << reader->GetImageIO()->GetNumberOfComponents() << std::endl;
  std::cout << "Number of Dimensions: " << reader->GetImageIO()->GetNumberOfDimensions() << std::endl;

  // TODO: Compare region attributes with input parameters.

  return EXIT_SUCCESS;
}
