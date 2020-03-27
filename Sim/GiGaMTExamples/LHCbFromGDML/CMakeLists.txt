################################################################################
# Package: LHCbFromGDML
# Simple setup to add the frozen LHCb geometry from a GDML file to Gaussino.
#
# Author: Dominik Muller, GDML file from Ben Couturier
#
################################################################################
gaudi_subdir(LHCbFromGDML v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiAlg
                         HepMC3
                         HepMCUser
                         Gen/GenInterfaces)

AddHepMC3()

gaudi_add_module(LHCbFromGDML
                  src/*.cpp
                  INCLUDE_DIRS GenInterfaces ${HEPMC3_INCLUDE_DIR} HepMCUser
                  LINK_LIBRARIES GaudiAlgLib ${HEPMC3_LIBRARIES} GenEvent)
add_dependencies(LHCbFromGDML HepMC3Ext)
