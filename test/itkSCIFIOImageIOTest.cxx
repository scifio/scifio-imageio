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

/**
 * Provides usage message and exits.
 */
int fail(char * argv [])
{
  std::cerr << "Usage: " << argv[0] << " input output [OPTIONS]\n"
            << "\n"
            << "OPTIONS:\n"
            << "-w, --write-scifio\n"
            << "\tEnable the SCIFIOImageIO to be used for output. By default, the standard ITK ImageIO are used for writing."
               << " This flag will allow Bio-Formats-specific formats to be written (e.g. ome.tiff).\n"
            << "-r, --rgb\n"
            << "\tEnabled RGB mode. The specified pixel type will be as an itk::RGBPixel.\n"
            << "-v <n>, --divs <n>\n"
            << "\tUse n streaming divisions\n"
            << "-s <n1 n2>, --series <n1 n2>\n"
            << "\tAfter reading the first series specified by @@ notation (0 default) will read all series from n1+1 to n2,"
               << " exclusive. NB: this flag is mutually exclusive with manual @series@ filename annotation.\n"
            << "-a, --all\n"
            << "\tProcesses all discovered series.\n"
            << "-d <2-5>, --dims <2-5>\n"
            << "\tSets the dimensionality. This should be equal to or less than your target image\'s dimensionality."
              << " If less, dimensions will be truncated in reverse dimension order\n"
            << "-t <T>, --type <T>\n"
            << "\tSets the pixel type. T should be one of: int, uint, char, uchar, short, ushort, float, long, double."
              << " Default is ushort.\n";
  return EXIT_FAILURE;
}

/**
 * Templated method performs the actual image io
 */
template < class PixelType, unsigned int NDimension >
int RunTest ( const char * inputFileName,
              const char * outputFileName,
              int seriesStart,
              int seriesEnd,
              std::string numberOfStreamDivisions,
              bool writeSCIFIO,
              bool allSeries )
{
  typedef itk::Image<PixelType, NDimension>             ImageType;
  typedef typename itk::ImageFileReader< ImageType >    ReaderType;

  itk::SCIFIOImageIO::Pointer io = itk::SCIFIOImageIO::New();
  io->DebugOn();

  typename ReaderType::Pointer reader = ReaderType::New();
  std::cout << "reader->GetUseStreaming(): " << reader->GetUseStreaming() << std::endl;
  std::cout << "done checking streaming usage" << std::endl;

  reader->SetImageIO( io );

  reader->SetFileName( inputFileName );

  typedef itk::StreamingImageFilter< ImageType, ImageType > StreamingFilter;
  typename StreamingFilter::Pointer streamer = StreamingFilter::New();
  streamer->SetInput( reader->GetOutput() );
  streamer->SetNumberOfStreamDivisions( atoi(numberOfStreamDivisions.c_str()) );

  typedef itk::ImageFileWriter< ImageType > WriterType;
  typename WriterType::Pointer writer;
  writer = WriterType::New();
  writer->SetInput( streamer->GetOutput() );

  if ( writeSCIFIO )
    {
    itk::SCIFIOImageIO::Pointer ioOut = itk::SCIFIOImageIO::New();
    ioOut->DebugOn();
    writer->SetImageIO( ioOut );
    }

  reader->UpdateOutputInformation();
  io->SetSeries(seriesStart);
  reader->Modified();

  if ( allSeries )
    {
    seriesEnd = io->GetSeriesCount();
    }

  bool insertSeries = seriesEnd > (seriesStart + 1);

  int seriesCount = seriesEnd - seriesStart;
  int sigFigs = 0;

  while (seriesCount >= 10)
    {
    seriesCount /= 10;
    sigFigs++;
    }

  while ( seriesStart < seriesEnd )
    {

    std::string fileOut = outputFileName;

    if (insertSeries)
      {
      // Adjust file names if converting multiple series
      std::stringstream ssout;

      int currentSigFigs = 0;
      int currentSeries = seriesStart;

      while (currentSeries >= 10)
        {
        currentSeries /= 10;
        currentSigFigs++;
        }

      for (int i=0; i<(sigFigs - currentSigFigs); i++)
        {
        ssout << 0;
        }

      ssout << seriesStart;
      fileOut.insert(fileOut.find_first_of('.'), ssout.str());
      }

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

    if ( seriesStart < seriesEnd)
      {
      io->SetSeries(seriesStart);
      reader->Modified();
      }
    }

  std::string notes;
  itk::ExposeMetaData<std::string>( reader->GetMetaDataDictionary(), "Recording #1 Notes", notes );
  std::cout << "Notes: " << notes << std::endl;

  return EXIT_SUCCESS;
}

/*
 * Helper method to type narrow the dimension
 */
template < class PixelType >
int ProcessDimension ( std::string dim, char * argv [],
                       const char * inputFileName,
                       const char * outputFileName,
                       int seriesStart,
                       int seriesEnd,
                       std::string numberOfStreamDivisions,
                       bool writeSCIFIO,
                       bool allSeries)
{
  if (dim == "2")
    {
    return RunTest <PixelType, 2> (inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
    }
  else if (dim == "3")
    {
    return RunTest <PixelType, 3> (inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
    }
  else if (dim == "4")
    {
    return RunTest <PixelType, 4> (inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
    }
  else if (dim == "5")
    {
    return RunTest <PixelType, 5> (inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
    }
  else
    {
    return fail(argv);
    }
}

/*
 * Helper method to type narrow pixel type based on RGB status
 */
template < class PixelType >
int ProcessRGB ( bool rgb, std::string dim,
                 char * argv [],
                 const char * inputFileName,
                 const char * outputFileName,
                 int seriesStart, int seriesEnd,
                 std::string numberOfStreamDivisions,
                 bool writeSCIFIO,
                 bool allSeries )
{
  if (rgb)
    {
    return ProcessDimension < itk::RGBPixel< PixelType > > (dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
    }
  return ProcessDimension < PixelType > (dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries);
}

/**
 * Main method
 */
int itkSCIFIOImageIOTest( int argc, char * argv [] )
{
  if( argc < 3)
    {
    return fail(argv);
    }
  const char * inputFileName = argv[1];
  const char * outputFileName = argv[2];

  // default flag values
  std::string numberOfStreamDivisions = "4";
  int seriesStart = 0;
  int seriesEnd = 1;

  std::string ptype = "ushort";
  std::string dim = "2";
  bool rgb = false;
  bool writeSCIFIO = false;
  bool allSeries = false;

  // parse flags
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
      i += 2;
      }
    else if (strcmp (argv[i], "-t") == 0 || strcmp (argv[i], "--type") == 0)
      {
      if (i + 1 >= argc)
        {
        return fail(argv);
        }
      ptype = argv[i+1];
      i++;

      }
    else if (strcmp (argv[i], "-a") == 0 || strcmp (argv[i], "--all") == 0)
      {
      allSeries = true;
      }
    else if (strcmp (argv[i], "-w") == 0 || strcmp (argv[i], "--write-scifio") == 0)
      {
      writeSCIFIO = true;
      }
    else if (strcmp (argv[i], "-r") == 0 || strcmp (argv[i], "--rgb") == 0)
      {
      rgb = true;
      }
    else if (strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--dims") == 0)
      {
      if (i + 1 >= argc)
        {
        return fail(argv);
        }
      dim = argv[i+1];
      }
    }

  // Narrow pixel type
  if (ptype == "int")
    {
    return ProcessRGB<int> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "uint")
    {
    return ProcessRGB<unsigned int> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "char")
    {
    return ProcessRGB<char> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "uchar")
    {
    return ProcessRGB<unsigned char> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "short")
    {
    return ProcessRGB<short> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "ushort")
    {
    return ProcessRGB<unsigned short> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "long")
    {
    return ProcessRGB<long> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "float")
    {
    return ProcessRGB<float> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else if (ptype == "double")
    {
    return ProcessRGB<double> ( rgb, dim, argv, inputFileName, outputFileName, seriesStart, seriesEnd, numberOfStreamDivisions, writeSCIFIO, allSeries );
    }
  else
    {
    return fail(argv);
    }
}
