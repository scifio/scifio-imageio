#!/bin/sh

script_dir="`cd $(dirname $0); pwd`"

docker run \
  --rm \
  -v $script_dir/../..:/usr/src/scifio-imageio \
    insighttoolkit/scifio-imageio-test \
      /usr/src/scifio-imageio/test/Docker/test.sh
