# SCIFIO ImageIO plugin

![](https://github.com/scifio/scifio-imageio/workflows/Build,%20test,%20package/badge.svg)

__NB: This `scifio-imageio` provides the _C++_ side of SCIFIO-ITK integration. For the _Java_ side, see [scifio-itk-bridge](https://github.com/scifio/scifio-itk-bridge).__

This package provides an ImageIO plugin for the
[Insight Toolkit](http://itk.org/) (ITK) that uses
[Bio-Formats](https://github.com/openmicroscopy/bioformats)
to read and write supported file formats.

## About ITK ImageIO

ITK provides an ImageIO plug-in structure that works via discovery through a
dependency injection scheme. This allows a program built on ITK to load
plug-ins for reading and writing different image types without actually linking
to the ImageIO libraries required for those types. Such encapsulation
automatically grants two major boons: firstly, programs can be easily extended
just by virtue of using ITK (developers do not have to specifically accommodate
or anticipate what plug-ins may be used). Secondly, the architecture provides a
distribution method for open source software, like Bio-Formats, which have
licenses that might otherwise exclude them from being used with other software
suites.

## Credits

The SCIFIO ImageIO plugin was developed by Gaetan Lehmann, Mark Hiner,
Curtis Rueden, Melissa Linkert and Matt McCormick. Development of this
module was funded in part by the
[FARSIGHT project](http://farsight-toolkit.org/), as well as the
[Open Microscopy Environment](http://openmicroscopy.org/).

Special thanks to Alex Gouaillard, Sebastien Barre, Luis Ibanez and
Jim Miller for fixes and suggestions.

## Prerequisites

You should have [CMake](http://www.cmake.org/) installed, to allow the
configuration of ITK builds. If you want the latest ITK development build, you
will need [Git](http://git-scm.com/) as well.

## Installation

Simply download ITK from the [Kitware software
page](http://www.itk.org/ITK/resources/software.html). Using CMake, set the
following configuration flag:
```
Module_SCIFIO = ON
```
Note that this flag is only visible in "advanced" mode within CMake.

If you would like to use the utility classes included with the SCIFIO imageIO,
also set the flag:
```
BUILD_TESTING = ON
```
Then build ITK as normal. It will automatically download and build the latest
SCIFIO imageIO plugin.

## Usage

Applications using the installed ITK should automatically defer to the SCIFIO
ImageIO, and thus Bio-Formats, when reading or saving images not natively
supported by ITK.

To use the SCIFIO test utility, run:
```
SCIFIOTestDriver
```
from your `${ITK_BUILD}/bin` directory. This program has four separate
applications that can be directly invoked using the syntax:
```
SCIFIOTestDriver [Program to run] [Program arguments]
```
The programs are as follows:

* __itkSCIFIOImageInfoTest__:
  Displays basic information to verify the SCIFIO imageIO works, using .fake
  images.
* __itkSCIFIOImageIOTest__:
  Reads an input image, and writes it out as a specified type
* __itkRGBSCIFIOImageTest__:
  Same as itkSCIFIOImageIOTest but for
  [RGB](http://www.itk.org/Doxygen/html/classitk_1_1RGBPixel.html) types
* __itkVectorImageSCIFIOImageIOTest__:
  Same as itkSCIFIOImageIOTest but for
  [VectorImage](http://www.itk.org/Doxygen/html/classitk_1_1VectorImage.html)
  type

For example, to convert a .czi image to a .tif, you would use:
```
SCIFIOTestDriver itkSCIFIOImageIOTest in.czi out.tif
```

## Troubleshooting

For general troubleshooting issues using this plugin, please e-mail the
[SCIFIO mailing list](http://scif.io/mailman/listinfo/scifio).

Any questions about the capabilities of Bio-Formats can be
[directed to the OME team](http://www.openmicroscopy.org/site/community).

For ITK questions, see the
[ITK mailing lists](http://www.itk.org/ITK/help/mailing.html).
