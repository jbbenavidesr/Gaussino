################################################################################
# Package: GenBeam
################################################################################
gaudi_subdir(GenBeam v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/GenEvent
                         NewRnd
                         HepMCUser
                         Gen/GenInterfaces
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel)

AddHepMC3()

gaudi_add_module(GenBeam
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces ${HEPMC3_INCLUDE_DIR} HepMCUser
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel NewRnd ${HEPMC3_LIBRARIES})

add_dependencies(GenBeam HepMC3Ext)
