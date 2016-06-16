# - Try to find CRMC
# Defines:
#
#  CRMC_FOUND
#  CRMC_HOME (not cached)
#  CRMC_INCLUDE_DIR
#  CRMC_INCLUDE_DIRS (not cached)
#  CRMC_LIBRARY
#  CRMC_LIBRARIES (not cached)
#  CRMC_LIBRARY_DIRS (not cached)


find_path(CRMC_INCLUDE_DIR CRMCconfig.h
          HINTS $ENV{CRMC_ROOT_DIR}/src
          PATH_SUFFIXES src)

if(CRMC_INCLUDE_DIR)
  get_filename_component(CRMC_HOME "${CRMC_INCLUDE_DIR}" DIRECTORY)
  set(CRMC_INCLUDE_DIRS "${CRMC_INCLUDE_DIR}")
  set(CRMC_LIBRARY_DIRS "${CRMC_HOME}/lib")
  set(CRMC_LIBRARIES "${CRMC_LIBRARY_DIRS}/libCrmcBasic.so" "${CRMC_LIBRARY_DIRS}/libHepEvtDummy.so")
  
endif()

mark_as_advanced(CRMC_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set CRMC_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CRMC DEFAULT_MSG CRMC_INCLUDE_DIR)

mark_as_advanced(CRMC_FOUND)

set(CRMC_ENVIRONMENT SET CRMC_TABS ${CRMC_HOME}/tabs)
