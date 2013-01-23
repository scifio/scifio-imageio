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
#include "itkMetaDataObject.h"
#include "itkStreamingImageFilter.h"

int itkSCIFIOImageIOTest( int argc, char * argv [] )
{
  if( argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << " input output [numberOfStreamDivisions]\n";
    return EXIT_FAILURE;
    }
  const char * inputFileName = argv[1];
  const char * outputFileName = argv[2];
  std::string numberOfStreamDivisions = "4";
  if( argc > 3 )
    {
    numberOfStreamDivisions = argv[3];
    }

  typedef unsigned char       PixelType;
  const unsigned int          Dimension = 3;

  typedef itk::Image< PixelType, Dimension >   ImageType;
  typedef itk::ImageFileReader< ImageType >    ReaderType;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();

  io->DebugOn();

  ReaderType::Pointer reader = ReaderType::New();
  std::cout << "reader->GetUseStreaming(): " << reader->GetUseStreaming() << std::endl;

  std::cout << "done checking streaming usage";

  reader->SetImageIO(io);
  /*
  reader->SetFileName("fileNotFound");

  // should get an exception
  // TODO: Exception is thrown in Java not c++ .. is not caught and dies
  bool catched = false;
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    catched = true;
    }
  if( !catched )
    {
    std::cerr << "exception not catched for wrong path" << std::endl;
    return EXIT_FAILURE;
    }
*/

  reader->SetFileName( inputFileName );

  typedef itk::StreamingImageFilter<ImageType, ImageType> StreamingFilter;
  StreamingFilter::Pointer streamer = StreamingFilter::New();
  streamer->SetInput( reader->GetOutput() );
  streamer->SetNumberOfStreamDivisions( atoi(numberOfStreamDivisions.c_str()) );

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer;
  //
  // Use the generic writers to write the image.
  //
  writer = WriterType::New();
  writer->SetInput( streamer->GetOutput() );
  writer->SetFileName( outputFileName );

  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }

  std::string notes;
  itk::ExposeMetaData<std::string>( reader->GetMetaDataDictionary(), "Recording #1 Notes", notes );
  std::cout << "Notes: " << notes << std::endl;

  return EXIT_SUCCESS;
}
