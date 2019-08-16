################################################################################
# Package: GiGaMTDebug
#
# Collection of G4 objects used for debugging
#################################################################################
gaudi_subdir(GiGaMTDebug v1r0)
gaudi_depends_on_subdirs(Sim/GiGaMTCore
                         Sim/GiGaMTFactories)

find_package(CLHEP COMPONENTS Random Vector)
find_package(XercesC)
AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

add_definitions(-DG4MULTITHREADED)
add_definitions(-DG4USE_STD11)

gaudi_add_module(GiGaMTDebug
        src/*.cpp
        INCLUDE_DIRS GiGaMTFactories
        LINK_LIBRARIES GaudiAlgLib GiGaMTCoreTruthLib)
