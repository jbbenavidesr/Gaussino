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
# - Try to find YODA
# Defines:
#
#  YODA_FOUND
#  YODA_HOME (not cached)
#  YODA_INCLUDE_DIRS (not cached)
#  YODA_LIBRARY
#  YODA_LIBRARIES (not cached)
#  YODA_LIBRARY_DIRS (not cached)


find_library(YODA_LIBRARY YODA
             HINTS $ENV{YODA_ROOT_DIR}/lib)

if(YODA_LIBRARY)
  get_filename_component(YODA_LIBRARY_DIRS "${YODA_LIBRARY}" DIRECTORY)
  get_filename_component(YODA_HOME "${YODA_LIBRARY_DIRS}" DIRECTORY)
  set(YODA_INCLUDE_DIRS "${YODA_HOME}/include")
  set(YODA_LIBRARIES "${YODA_LIBRARY}")
  
endif()

mark_as_advanced(YODA_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set YODA_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YODA DEFAULT_MSG YODA_LIBRARY)

mark_as_advanced(YODA_FOUND)

set(YODA_PYTHON_PATH ${YODA_HOME}/lib/python${Python_config_version_twodigit}/site-packages)
set(YODA_BINARY_PATH ${YODA_HOME}/bin)
