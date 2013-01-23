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
#include "itkSCIFIOImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkSCIFIOImageIO.h"
#include "itkVersion.h"

namespace itk
{
SCIFIOImageIOFactory::SCIFIOImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkSCIFIOImageIO",
                         "SCIFIO Image IO",
                         1,
                         CreateObjectFunction<SCIFIOImageIO>::New());
}

SCIFIOImageIOFactory::~SCIFIOImageIOFactory()
{}

const char*
SCIFIOImageIOFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char*
SCIFIOImageIOFactory::GetDescription() const
{
  return "SCIFIO ImageIO Factory, allows the loading of "
         "SCIFIO-compatible images into Insight; see "
         "http://openmicroscopy.org/site/support/bio-formats/users/itk";
}

} // end namespace itk
