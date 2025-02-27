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

fullversion="$major.$minor.$patch$postfix_dash"

cat <<C > $target
#pragma once

#define EDBEE_VERSION "$fullversion"

#define EDBEE_VERSION_MAJOR $major
#define EDBEE_VERSION_MINOR $minor
#define EDBEE_VERSION_PATCH $patch
#define EDBEE_VERSION_POSTFIX "$postfix"
C

echo $target
echo "---------------"
cat $target

# Update the Doxyfile
doxy_target="../edbee-lib/Doxyfile"
sed -i '' "s/^PROJECT_NUMBER[[:space:]]*=.*/PROJECT_NUMBER = \"v$fullversion\"/" $doxy_target
sed -i '' "s/^OUTPUT_DIRECTORY[[:space:]]*=.*/OUTPUT_DIRECTORY = \"..\/edbee-lib-doxydocs\/v$fullversion\"/" $doxy_target


