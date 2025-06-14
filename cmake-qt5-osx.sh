#!/bin/sh

cmake -DBUILD_WITH_QT5=1 -DCMAKE_PREFIX_PATH="$(brew --prefix qt@5)" -DCMAKE_OSX_SYSROOT=macosx
make
