################################################################################
# Package: GiGaMTCore 
#
# This package should contain all modified core Geant4 classes (e.g.
# run managers). While the Gaudi cmake plugins are used to handle this package,
# none of the modified classes should depend on anything Gaudi related.
#################################################################################
gaudi_subdir(GiGaMTCore v1r0)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_library(GiGaMTCoreLib
                 src/*.cpp
                 PUBLIC_HEADERS GiGaMTCore
                 INCLUDE_DIRS CLHEP Geant4 XercesC
                 LINK_LIBRARIES CLHEP ${GEANT4_LIBS} XercesC)
