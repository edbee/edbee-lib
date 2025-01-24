#!/bin/sh

if [ "$#" -lt 3 ]; then
  echo "Generates the edbee version header"
  echo ""
  echo "Usage: $0 <major> <minor> <patch> [<postfix>]"
  exit;
fi

major="$1"
minor="$2"
patch="$3"
postfix="$4"
if [ -n "$4" ]; then
  postfix_dash="-$4"
fi
target="../edbee-lib/edbee/edbeeversion.h"

cat <<C > $target
#pragma once

#define EDBEE_VERSION "$major.$minor.$patch$postfix_dash"

#define EDBEE_VERSION_MAJOR $major
#define EDBEE_VERSION_MINOR $minor
#define EDBEE_VERSION_PATCH $patch
#define EDBEE_VERSION_POSTFIX "$postfix"
C

cat $target
