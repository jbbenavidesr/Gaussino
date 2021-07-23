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
#  Try to find POWHEG-BOX
# Defines:
#
#  POWHEG-BOX_BINARY_PATH

find_path(POWHEG-BOX_BINARY_PATH NAMES ZZ Dijet
          HINTS ${powhegbox_home}/bin 
          PATH_SUFFIXES bin)

# handle the QUIETLY and REQUIRED arguments and set POWHEG-BOX_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(POWHEG-BOX DEFAULT_MSG POWHEG-BOX_BINARY_PATH)

mark_as_advanced(POWHEG-BOX_FOUND)
