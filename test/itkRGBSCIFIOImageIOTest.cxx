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
#include "itkSCIFIOImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkMetaDataObject.h"
#include "itkStreamingImageFilter.h"

int
itkRGBSCIFIOImageIOTest(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input output\n";
    return EXIT_FAILURE;
  }

  using PixelType = itk::RGBPixel<unsigned char>;
  constexpr unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<ImageType>;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();

  io->DebugOn();

  ReaderType::Pointer reader = ReaderType::New();
  std::cout << "reader->GetUseStreaming(): " << reader->GetUseStreaming() << std::endl;

  reader->SetFileName(argv[1]);
  reader->SetImageIO(io);

  using StreamingFilter = itk::StreamingImageFilter<ImageType, ImageType>;
  StreamingFilter::Pointer streamer = StreamingFilter::New();
  streamer->SetInput(reader->GetOutput());
  streamer->SetNumberOfStreamDivisions(3);

  itk::ImageFileWriter<ImageType>::Pointer writer;

  //
  // Use the generic writers to write the image.
  //
  writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetInput(streamer->GetOutput());
  writer->SetFileName(argv[2]);

  try
  {
    streamer->Update();
    writer->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }

  std::string notes;
  itk::ExposeMetaData<std::string>(reader->GetMetaDataDictionary(), "Recording #1 Notes", notes);
  std::cout << "Notes: " << notes << std::endl;

  return EXIT_SUCCESS;
}
