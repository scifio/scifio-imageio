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
#ifndef itkSCIFIOImageIO_h
#define itkSCIFIOImageIO_h

#include "SCIFIOExport.h"
#include "itkStreamingImageIOBase.h"

#include "itksys/Process.h"
#include "itksys/SystemTools.hxx"

#include <sstream>

namespace itk
{
/** \class SCIFIOImageIO
 *
 * \brief Interface to the OME SCIFIO Java Library.
 *
 * This class provides the ability to read and write all the file formats
 * supported by the [SCIFIO] Java library, including [Bio-Formats].
 *
 * It invokes a Java process via a system call, and uses pipes to
 * communicate with it.
 *
 * The SCIFIO ImageIO module has the following runtime requirements:
 *
 * - Java Runtime Environment (JRE)
 * - SCIFIO Java libraries (i.e., JAR files) -- for open [file formats].
 *   SCIFIO (which stands for "SCientific Image Formats Input and Output)
 *   is a framework for image I/O in Java. It is permissively licensed
 *   (BSD-2) and includes support for reading and writing several formats.
 * - Optionally, Bio-Formats Java libraries -- for additional [file formats].
 *   Bio-Formats is an extension of SCIFIO, licensed under the GPL, which
 *   provides support for a large number (more than 120) proprietary file
 *   formats (PFFs), particularly those in life sciences disciplines.
 *
 * Note that none of the above are required at build time. By default, the
 * Bio-Formats Java libraries (bioformats_package.jar, which is a bundle of all
 * SCIFIO & Bio-Formats libraries including dependencies) are downloaded at
 * build time and placed into the lib/Jars subfolder of the build tree.
 * A distributable JRE is also downloaded at build time.
 *
 * The following environment variables can optionally be set to control the
 * behavior of the SCIFIO ImageIO plugin -- in particular, how it interfaces
 * with the Java Runtime Environment:
 *
 * - JAVA_HOME - Environment variable to determine the location of the java
 *   binary. If not set, "java" must be on the system path.
 * - SCIFIO_PATH - Specifies the location of the required SCIFIO JAR
 *   libraries. If unset, the default behavior is to first check the
 *   lib/jars subfolder of the build directory. Failing that, it looks for
 *   the same subfolder in the install directory given at build time.
 * - JAVA_FLAGS - Used to pass any additional desired parameters to the Java
 *   execution. This is especially useful to override Java's maximum heap
 *   size, but also nice for tweaking the VM in many other ways (e.g.,
 *   garbage collection settings).
 *
 * [scifio]:       http://openmicroscopy.org/site/support/bio-formats/developers/scifio.html
 * [bio-formats]:  http://openmicroscopy.org/site/products/bio-formats
 * [file formats]: http://openmicroscopy.org/site/support/bio-formats/formats
 *
 * \ingroup SCIFIO
 */
class SCIFIO_EXPORT SCIFIOImageIO : public StreamingImageIOBase
{
public:
  typedef SCIFIOImageIO               Self;
  typedef ImageIOBase                 Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory **/
  itkNewMacro(Self);

  /** RTTI (and related methods) **/
  itkTypeMacro(SCIFIOImageIO, Superclass);

  bool SupportsDimension( unsigned long dim ) override;

  /**--------------- Read the data----------------- **/

  bool CanReadFile(const char* FileNameToRead) override;

  /* Sets the series to read in a multi-series dataset */
  virtual bool SetSeries(int series);

  /* Sets the series to read in a multi-series dataset */
  virtual int GetSeriesCount();

  /* Set the spacing and dimension information for the set file name */
  void ReadImageInformation() override;

  /* Read the data from the disk into provided memory buffer */
  void Read(void* buffer) override;

  /**---------------Write the data------------------**/

  bool CanWriteFile(const char* FileNameToWrite) override;

  /* Set the spacing and dimension information for the set file name */
  void WriteImageInformation() override;

  /* Write the data to the disk from the provided memory buffer */
  void Write(const void* buffer) override;

protected:
  SCIFIOImageIO();
  ~SCIFIOImageIO() override;

  SizeType GetHeaderSize() const override { return 0; }

private:
  void CreateJavaProcess();
  void DestroyJavaProcess();
  std::string FindDimensionOrder(const ImageIORegion & region );
  std::string WaitForNewLines(int pipedatalength);
  void CheckError(std::string message);
  bool CheckJavaPath(std::string javaHome, std::string &javaCmd);
  std::string RemoveFinalSlash(std::string path) const;

  char ** toCArray( std::vector< std::string > & args )
    {
    char **argv = new char *[args.size() + 1];
    for( int i = 0; i < static_cast< int >( args.size() ); i++ )
      {
      itkDebugMacro( "SCIFIOImageIO::toCArray::args["<<i<<"] = " << args[i]);
      argv[i] = (char*)args[i].c_str();
      }
    argv[args.size()] = NULL;
    return argv;
    }

  ImageIOBase::IOComponentType scifioToITKComponentType( int pixelType )
    {
    switch ( pixelType )
      {
      case 0:
        return CHAR;
      case 1:
        return UCHAR;
      case 2:
        return SHORT;
      case 3:
        return USHORT;
      case 4:
        return INT;
      case 5:
         return UINT;
      case 6:
        return FLOAT;
      default:
        return DOUBLE;
      }
    }

  int itkToSCIFIOPixelType( ImageIOBase::IOComponentType cmp )
  {
    switch ( cmp )
    {
    case CHAR:
      return 0;
    case UCHAR:
      return 1;
    case SHORT:
      return 2;
    case USHORT:
      return 3;
    case INT:
      return 4;
    case UINT:
      return 5;
    case FLOAT:
      return 6;
    case LONG:
    case ULONG:
    case DOUBLE:
    default:
      return 7;
    }
  }

  MetaDataDictionary           m_MetaDataDictionary;
  std::vector< std::string >   m_Args;
  char **                      m_Argv;
  itksysProcess_Pipe_Handle    m_Pipe[2];
  itksysProcess *              m_Process;
};
} // end namespace itk

#endif // itkSCIFIOImageIO_h
