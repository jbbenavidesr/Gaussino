################################################################################
# Package: GiGaMTCore 
#
# This package should contain all modified core Geant4 classes (e.g.
# run managers). While the Gaudi cmake plugins are used to handle this package,
# none of the modified classes should depend on anything Gaudi related.
#################################################################################
gaudi_subdir(GiGaMTCore v1r0)
gaudi_depends_on_subdirs(NewRnd
                         Event/MCEvent  # FIXME: Don't like this
                         HepMCUser)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)
find_package(HepMC CONFIG)
find_package(ROOT COMPONENTS MathCore GenVector)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DTRUTHDEBUG)
endif()

gaudi_add_library(GiGaMTCoreTruthLib
                 src/truth/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS HepMC CLHEP Geant4 XercesC NewRnd HepMCUtils
                 LINK_LIBRARIES HepMC CLHEP ${GEANT4_LIBS} XercesC ${ROOT_LIBRARIES} HepMCUtils MCEvent)

gaudi_add_library(GiGaMTCoreRunLib
                 src/run/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS HepMC CLHEP Geant4 XercesC NewRnd
                 LINK_LIBRARIES HepMC CLHEP ${GEANT4_LIBS} XercesC GiGaMTCoreTruthLib )
