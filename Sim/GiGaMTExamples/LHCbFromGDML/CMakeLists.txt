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
                         Gen/GenInterfaces)

find_package(HepMC CONFIG)

gaudi_add_module(LHCbFromGDML
                  src/*.cpp
                  INCLUDE_DIRS GenInterfaces HepMC
                  LINK_LIBRARIES GaudiAlgLib HepMC)
