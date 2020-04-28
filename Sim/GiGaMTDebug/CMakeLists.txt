################################################################################
# Package: GiGaMTDebug
#
# Collection of G4 objects used for debugging
#################################################################################
gaudi_subdir(GiGaMTDebug v1r0)
gaudi_depends_on_subdirs(Sim/GiGaMTCore
                         Sim/GiGaMTFactories)

AddHepMC3()
find_package(ROOT COMPONENTS MathCore GenVector)

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()

gaudi_add_module(GiGaMTDebug
        src/*.cpp
        INCLUDE_DIRS GiGaMTFactories
        LINK_LIBRARIES GaudiAlgLib GiGaMTCoreTruthLib)
