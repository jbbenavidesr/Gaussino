################################################################################
# Package: GenBeam
################################################################################
gaudi_subdir(GenBeam v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/GenEvent
                         NewRnd
                         Gen/GenInterfaces
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel)

find_package(HepMC CONFIG)

gaudi_add_module(GenBeam
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces ${HEPMC_INCLUDE_DIR}
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel NewRnd ${HEPMC_LIBRARIES})
