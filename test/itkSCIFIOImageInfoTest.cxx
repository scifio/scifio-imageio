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
#include <iostream>
#include <vector>
#include <string>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataObject.h"
#include "itkMetaDataDictionary.h"
#include "itkImageIOBase.h"
//#include "itkSCIFIOImageIO.h"

#define METADATA_NOT_FOUND "No value for this key."

int main( int argc, char * argv[] )
{
  if( argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " input\n";
    return EXIT_FAILURE;
  }

  typedef unsigned char     PixelType;
  const unsigned int        Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  ImageType;

  typedef itk::ImageFileReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  ImageType::Pointer img;

  reader->SetFileName(argv[1]);
  reader->Update();
  img = reader->GetOutput();

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
}
