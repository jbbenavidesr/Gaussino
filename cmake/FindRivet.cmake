###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
# - Try to find Rivet
# Defines:
#
#  RIVET_FOUND
#  RIVET_INCLUDE_DIR
#  RIVET_INCLUDE_DIRS (not cached)
#  RIVET_LIBRARY
#  RIVET_LIBRARIES (not cached)
#  RIVET_LIBRARY_DIRS (not cached)

find_library(RIVET_LIBRARY NAMES Rivet)

find_path(RIVET_INCLUDE_DIR Rivet/Rivet.hh)

find_program(RIVET_EXECUTABLE NAMES rivet)
find_program(RIVET_buildplugin_EXECUTABLE NAMES rivet-buildplugin)

mark_as_advanced(RIVET_INCLUDE_DIR RIVET_LIBRARY RIVET_EXECUTABLE RIVET_buildplugin_EXECUTABLE)

# handle the QUIETLY and REQUIRED arguments and set Rivet_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Rivet DEFAULT_MSG RIVET_INCLUDE_DIR RIVET_LIBRARY RIVET_EXECUTABLE RIVET_buildplugin_EXECUTABLE)

set(RIVET_LIBRARIES ${RIVET_LIBRARY})
get_filename_component(RIVET_LIBRARY_DIRS ${RIVET_LIBRARY} PATH)

set(RIVET_INCLUDE_DIRS ${RIVET_INCLUDE_DIR})

get_filename_component(RIVET_BINARY_PATH ${RIVET_EXECUTABLE} PATH)

mark_as_advanced(Rivet_FOUND)
