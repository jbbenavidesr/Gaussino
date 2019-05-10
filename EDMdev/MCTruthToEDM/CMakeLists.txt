gaudi_subdir(MCTruthToEDM v1r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         Sim/GiGaMTCore
                         Defaults
                         HepMC3
                         HepMCUser)

AddHepMC3()


gaudi_add_module(MCTruthToEDM
                 src/*.cpp
                 INCLUDE_DIRS ${HEPMC_INCLUDE_DIR} Defaults
                 LINK_LIBRARIES GiGaMTCoreTruthLib MCEvent)

add_dependencies(MCTruthToEDM HepMC3Ext)
