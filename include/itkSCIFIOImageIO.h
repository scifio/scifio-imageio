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
#ifndef __itkSCIFIOImageIO_h
#define __itkSCIFIOImageIO_h

#include "itkImageIOBase.h"
#include "itkStreamingImageIOBase.h"
#include <sstream>
#include <iostream>
#include "itksys/Process.h"
#include "itksys/SystemTools.hxx"

namespace itk
{
/** \class SCIFIOImageIO
 *
 * \brief Interface to the SCIFIO Java Library.
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
 * - SCIFIO Java libraries (i.e., JAR files) -- for open [file formats]
 * - optionally, Bio-Formats Java libraries -- for additional [file formats]
 *
 * Note that none of the above are required at build time.  By default, the
 * Bio-Formats Java libraries (loci_tools.jar) are downloaded at build time.
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
 * \ingroup ITKIOSCIFIO
 */
class ITK_EXPORT SCIFIOImageIO : public StreamingImageIOBase
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

  /**--------------- Read the data----------------- **/
  virtual bool CanReadFile(const char* FileNameToRead);
  /* Set the spacing and dimension information for the set file name */
  virtual void ReadImageInformation();
  /* Read the data from the disk into provided memory buffer */
  virtual void Read(void* buffer);

  /**---------------Write the data------------------**/
  virtual bool CanWriteFile(const char* FileNameToWrite);
  /* Set the spacing and dimension information for the set file name */
  virtual void WriteImageInformation();
  /* Write the data to the disk from the provided memory buffer */
  virtual void Write(const void* buffer);

  void CreateJavaProcess();
  void DestroyJavaProcess();

protected:
  SCIFIOImageIO();
  ~SCIFIOImageIO();

  virtual SizeType GetHeaderSize() const { return 0; }

private:
  typedef itk::ImageIOBase::IOComponentType  ITKComponent;
  typedef itk::MetaDataDictionary            ITKMeta;

  ITKMeta m_Meta;

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

  ITKComponent scifioToTIKComponentType( int pixelType ) {
    switch ( pixelType )
    {
    case 0:
      return CHAR;
    case 1:
      return UCHAR;
    case 2:
      return INT;
    case 3:
      return UINT;
    case 4:
      return LONG;
    case 5:
       return ULONG;
    case 6:
      return FLOAT;
    default:
      return DOUBLE;
    }
  }

  int itkToSCIFIOPixelType( ITKComponent cmp )
  {
    switch ( cmp )
    {
    case CHAR:
      return 0;
    case UCHAR:
      return 1;
    case SHORT:
    case INT:
      return 2;
    case USHORT:
    case UINT:
      return 3;
    case LONG:
      return 4;
    case ULONG:
      return 5;
    case FLOAT:
      return 6;
    default:
      return 7;
    }
  }

  std::vector< std::string >   m_Args;
  char **                      m_Argv;
  itksysProcess_Pipe_Handle    m_Pipe[2];
  itksysProcess *              m_Process;
};
} // end namespace itk

#endif // __itkSCIFIOImageIO_h
