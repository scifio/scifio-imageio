FROM debian:8
MAINTAINER Insight Software Consortium <community@itk.org>

RUN apt-get update && apt-get install -y \
  build-essential \
  curl \
  cmake \
  git \
  libexpat1-dev \
  libhdf5-dev \
  libjpeg-dev \
  libpng12-dev \
  libpython3-dev \
  libtiff5-dev \
  openjdk-7-jdk \
  openjdk-7-jre \
  python3 \
  python3-pip \
  ninja-build \
  wget \
  vim \
  zlib1g-dev

WORKDIR /usr/src

RUN git clone git://itk.org/ITK.git && \
  cd ITK && \
  git checkout v4.8.0 && \
  cd ../ && \
  mkdir ITK-build && \
  cd ITK-build && \
  cmake \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DBUILD_TESTING:BOOL=ON \
    -DBUILD_SHARED_LIBS:BOOL=ON \
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON \
    -DCMAKE_SKIP_RPATH:BOOL=ON \
    -DITK_LEGACY_REMOVE:BOOL=ON \
    -DITK_BUILD_DEFAULT_MODULES:BOOL=ON \
    -DITK_USE_SYSTEM_LIBRARIES:BOOL=ON \
    -DModule_SCIFIO:BOOL=ON \
    ../ITK
