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
                         HepMC3
                         HepMCUser)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)
AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DTRUTHDEBUG)
endif()

gaudi_add_library(GiGaMTCoreMessageLib
                 src/message/*.cpp
                 PUBLIC_HEADERS GiGaMTCore)

gaudi_add_library(GiGaMTCoreTruthLib
                 src/truth/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS ${HEPMC3_INCLUDE_DIR} CLHEP Geant4 XercesC NewRnd HepMCUtils
                 LINK_LIBRARIES ${HEPMC3_LIBRARIES} CLHEP ${GEANT4_LIBS} XercesC ${ROOT_LIBRARIES} HepMCUtils MCEvent  GiGaMTCoreMessageLib)

add_dependencies(GiGaMTCoreTruthLib HepMC3Ext)

gaudi_add_library(GiGaMTCoreRunLib
                 src/run/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS ${HEPMC3_INCLUDE_DIR} CLHEP Geant4 XercesC NewRnd
                 LINK_LIBRARIES ${HEPMC3_LIBRARIES} CLHEP ${GEANT4_LIBS} XercesC GiGaMTCoreTruthLib GiGaMTCoreMessageLib)

gaudi_add_library(GiGaMTCoreCutLib
                 src/cut/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS CLHEP Geant4
                 LINK_LIBRARIES CLHEP ${GEANT4_LIBS}  GiGaMTCoreMessageLib)
