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

int fail(char * argv [])
{
  std::cerr << "Usage: " << argv[0] << " input output [OPTIONS]\n\nOPTIONS:\n-v <n>, --divs <n>\n\tUse n streaming divisions\n-s <n1 n2>, --series <n1 n2>\n\tAfter reading the first series specified by @@ notation (0 default) will read all series from n1+1 to n2, exclusive\n-d <2-5>, --dims <2-5>\n\tSets the dimensionality. This should be equal to or less than your target image\'s dimensionality. If less, dimensions will be truncated in reverse dimension order\n-t <T>, --type <T>\n\tSets the pixel type. T should be one of: int, uint, short, ushort, float, long, double. Default is ushort.\n";
  return EXIT_FAILURE;
}

int itkSCIFIOImageIOTest( int argc, char * argv [] )
{
  if( argc < 3)
    {
    return fail(argv);
    }
  const char * inputFileName = argv[1];
  const char * outputFileName = argv[2];

  std::string numberOfStreamDivisions = "4";
  int seriesStart = 0;
  int seriesEnd = 1;
  typedef unsigned short       PixelType;
  typedef itk::Image< PixelType, 2 >   ImageType;

  for (int i = 3; i < argc; i++)
    {
    if (strcmp (argv[i], "-v") == 0 || strcmp (argv[i], "--divs") == 0)
      {
      if (i + 1 >= argc)
        {
        return fail(argv);
        }
      numberOfStreamDivisions = argv[i+1];
      i++;
      }
    else if (strcmp (argv[i], "-s") == 0 || strcmp (argv[i], "--series") == 0)
      {
      if (i + 2 >= argc)
        {
        return fail(argv);
        }
      seriesStart = atoi(argv[i+1]);
      seriesEnd = atoi(argv[i+2]);
      i+=2;
      }
    else if (strcmp (argv[i], "-t") == 0 || strcmp (argv[i], "--type") == 0)
      {
      if (i + 1 >= argc)
        {
        return fail(argv);
        }
      std::string ptype = argv[i+1];
      i++;

      if (ptype == "int")
        {
        typedef int                  PixelType;
        }
      else if (ptype == "uint")
        {
        typedef unsigned int         PixelType;
        }
      else if (ptype == "short")
        {
        typedef short                PixelType;
        }
      else if (ptype == "ushort")
        {
        typedef unsigned short       PixelType;
        }
      else if (ptype == "long")
        {
        typedef long                 PixelType;
        }
      else if (ptype == "float")
        {
        typedef float                PixelType;
        }
      else if (ptype == "double")
        {
        typedef double               PixelType;
        }
      else
        {
        return fail(argv);
        }
      }
    }

  // Have to check dimension size after PixelType, since the ImageType is being redefined.
  for (int i = 3; i < argc; i++)
    {
    if (strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--dims") == 0)
      {
      if (i + 1 >= argc)
        {
        return fail(argv);
        }
      std::string dim = argv[i+1];
      i++;

      if (dim == "2")
        {
        typedef itk::Image< PixelType, 2 >   ImageType;
        }
      else if (dim == "3")
        {
        typedef itk::Image< PixelType, 3 >   ImageType;
        }
      else if (dim == "4")
        {
        typedef itk::Image< PixelType, 4 >   ImageType;
        }
      else if (dim == "5")
        {
        typedef itk::Image< PixelType, 5 >   ImageType;
        }
      }
    }

  typedef itk::ImageFileReader< ImageType >    ReaderType;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();

  io->DebugOn();

  ReaderType::Pointer reader = ReaderType::New();
  std::cout << "reader->GetUseStreaming(): " << reader->GetUseStreaming() << std::endl;
  std::cout << "done checking streaming usage" << std::endl;

  reader->SetImageIO(io);

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

  while ( seriesStart < seriesEnd )
    {

    std::stringstream ss;
    ss << seriesStart;

    std::string fileOut = ss.str() + outputFileName;

    writer->SetFileName( fileOut );

    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &e)
      {
      std::cerr << e << std::endl;
      return EXIT_FAILURE;
      }
    seriesStart++;

    if (seriesStart < seriesEnd)
      {
      io->SetSeries(seriesStart);
      }
    }

  std::string notes;
  itk::ExposeMetaData<std::string>( reader->GetMetaDataDictionary(), "Recording #1 Notes", notes );
  std::cout << "Notes: " << notes << std::endl;

  return EXIT_SUCCESS;
}
