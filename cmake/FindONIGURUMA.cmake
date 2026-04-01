# Find the Oniguruma regex library.
#
# This module exports the following targets that should be used to link against:
# ONIGURUMA::ONIGURUMA
#
# Sets the usual variables expected for find_package scripts:
#
# ONIGURUMA_INCLUDE_DIR - header location
# ONIGURUMA_LIBRARIES - library to link against
# ONIGURUMA_FOUND - true if Oniguruma was found.

find_package(PkgConfig)

pkg_search_module(PC_ONIGURUMA onig libonig oniguruma liboniguruma)

find_path(ONIGURUMA_INCLUDE_DIR
  oniguruma.hpp
  oniguruma.h
  HINTS
    ${ONIGURUMA_DIR}
    $ENV{ONIGURUMAL_DIR}
    ${PC_ONIGURUMA_INCLUDE_DIRS}
  PATHS
    ${ONIGURUMA_HOME}/include
    /usr/local/include
    /usr/local/include/oniguruma
  )

find_library(
  ONIGURUMA_LIBRARY
  NAMES oniguruma onig
  HINTS
    ${ONIGURUMA_DIR}
    $ENV{ONIGURUMA_DIR}
    ${PC_ONIGURUMA_LIBRARY_DIRS}
        ${PC_ONIGURUMA_LIBRARY_DIR}
  PATHS
    ${ONIGURUMA_HOME}/lib
    /usr/local/lib
    /usr/local/lib/onig
    /usr/local/lib/oniguruma
  )

if(PC_ONIGURUMA_oniguruma_FOUND)
  set(ONIGURUMA_VER ${PC_ONIGURUMA_oniguruma_VERSION})
elseif(PC_ONIGURUMA_liboniguruma_FOUND)
  set(ONIGURUMA_VER ${PC_ONIGURUMA_liboniguruma_VERSION})
else()
  set(ONIGURUMA_VER ${PC_ONIGURUMA_VERSION})
endif()

include(FindPackageHandleStandardArgs)
# Support the REQUIRED and QUIET arguments, and set PUGIXML_FOUND if found.
find_package_handle_standard_args(ONIGURUMA REQUIRED_VARS ONIGURUMA_LIBRARY
                                  ONIGURUMA_INCLUDE_DIR VERSION_VAR ONIGURUMA_VER)

if(ONIGURUMA_FOUND AND NOT TARGET ONIGURUMA::ONIGURUMA)
  add_library(ONIGURUMA::ONIGURUMA UNKNOWN IMPORTED)
  set_target_properties(
    ONIGURUMA::ONIGURUMA
    PROPERTIES IMPORTED_LOCATION "${ONIGURUMA_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${ONIGURUMA_INCLUDE_DIR}")
endif()

mark_as_advanced(ONIGURUMA_LIBRARY ONIGURUMA_INCLUDE_DIR)
