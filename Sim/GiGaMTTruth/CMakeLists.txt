################################################################################
# Package: GiGaMTTruth 
#
# This package contains all code related to the tracking and extraction of truth
# information from the Geant4 propagation
#################################################################################
gaudi_subdir(GiGaMTTruth v1r0)
gaudi_depends_on_subdirs(NewRnd
                         GaudiAlg
                         HepMCUser
                         Kernel/PartProp
                         Sim/GiGaMTCore)

if(${Geant4_config_version} VERSION_LESS "10.06")
  add_definitions(-DG4MULTITHREADED)
  add_definitions(-DG4USE_STD11)
endif()


AddHepMC3()

FindG4libs(digits_hits event geometry global graphics_reps materials persistency
           particles processes run tracking track intercoms physicslists)

gaudi_install_headers(GiGaMTTruth)
