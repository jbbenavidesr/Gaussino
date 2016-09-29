# - Try to find Pythia8
# Defines:
#
#  PYTHIA8_FOUND
#  PYTHIA8_VERSION
#  PYTHIA8_INCLUDE_DIR
#  PYTHIA8_INCLUDE_DIRS (not cached)
#  PYTHIA8_LIBRARY
#  PYTHIA8_LIBRARIES (not cached)
#  PYTHIA8_LIBRARY_DIRS (not cached)
#  PYTHIA8XML
#  PYTHIA8DATA

find_path(PYTHIA8_INCLUDE_DIR Pythia.h Pythia8/Pythia.h
          HINTS $ENV{PYTHIA8_ROOT_DIR}/include ${PYTHIA8_ROOT_DIR}/include
          PATH_SUFFIXES include)

find_file(PYTHIA8XML xmldoc PATH_SUFFIXES ../share
          HINTS ${PYTHIA8_INCLUDE_DIR}/.. $ENV{PYTHIA8_ROOT_DIR} ${PYTHIA8_ROOT_DIR})

mark_as_advanced(PYTHIA8_INCLUDE_DIR PYTHIA8XML)

if(PYTHIA8XML AND NOT PYTHIA8_VERSION)
  file(READ ${PYTHIA8XML}/Version.xml versionstr)
  string(REGEX REPLACE ".*Pythia:versionNumber.*default.*[0-9][.]([0-9]+).*" "\\1" PYTHIA8_VERSION "${versionstr}")
  set(PYTHIA8_VERSION ${PYTHIA8_VERSION} CACHE STRING "Detected version of Pythia8.")
  mark_as_advanced(PYTHIA8_VERSION)
endif()

# Define the additional environment needed
set(PYTHIA8DATA ${PYTHIA8XML})

# Enforce a minimal list if none is explicitly requested
if(NOT Pythia8_FIND_COMPONENTS)
  set(Pythia8_FIND_COMPONENTS pythia8)
endif()

# component alternative names
set(_pythia8tohepmc_names hepmcinterface)
set(_hepmcinterface_names pythia8tohepmc)

foreach(component ${Pythia8_FIND_COMPONENTS})
  find_library(PYTHIA8_${component}_LIBRARY NAMES ${component} ${_${component}_names}
               HINTS $ENV{PYTHIA8_ROOT_DIR}/lib ${PYTHIA8_ROOT_DIR}/lib)
  if (PYTHIA8_${component}_LIBRARY)
    set(PYTHIA8_${component}_FOUND 1)
    list(APPEND PYTHIA8_LIBRARIES ${PYTHIA8_${component}_LIBRARY})

    get_filename_component(libdir ${PYTHIA8_${component}_LIBRARY} PATH)
    list(APPEND PYTHIA8_LIBRARY_DIRS ${libdir})
  else()
    set(PYTHIA8_${component}_FOUND 0)
  endif()
  mark_as_advanced(PYTHIA8_${component}_LIBRARY)
endforeach()

if(PYTHIA8_LIBRARY_DIRS)
  list(REMOVE_DUPLICATES PYTHIA8_LIBRARY_DIRS)
endif()

# handle the QUIETLY and REQUIRED arguments and set PYTHIA8_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pythia8 DEFAULT_MSG PYTHIA8_INCLUDE_DIR PYTHIA8_LIBRARIES PYTHIA8XML)

set(PYTHIA8_INCLUDE_DIRS ${PYTHIA8_INCLUDE_DIR})

mark_as_advanced(PYTHIA8_FOUND)
