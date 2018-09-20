################################################################################
# Package: GaussAlgs
################################################################################
gaudi_subdir(NewRnd v1r0)

find_package(CLHEP)

# Decide whether to link against CLHEP:
set( clhep_lib )
if( CLHEP_FOUND )
   set( clhep_lib CLHEP )
endif()

if( CLHEP_FOUND )
   include_directories( SYSTEM ${CLHEP_INCLUDE_DIRS} )
endif()

gaudi_depends_on_subdirs(GaudiAlg)

gaudi_add_library(NewRnd
  src/*.cpp
  PUBLIC_HEADERS Generators ${clhep_lib}
  LINK_LIBRARIES GaudiAlgLib ${clhep_lib})
