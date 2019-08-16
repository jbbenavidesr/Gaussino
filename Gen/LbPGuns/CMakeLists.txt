################################################################################
# Package: LbPGuns
################################################################################
gaudi_subdir(LbPGuns v6r1)

gaudi_depends_on_subdirs(
                         #FIXME: Det/DetDesc
                         Event/GenEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Gen/GenInterfaces
                         Gen/Generators
                         HepMC3
                         )

AddHepMC3()

gaudi_add_module(LbPGuns
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces ${HEPMC3_INCLUDE_DIR}
                 LINK_LIBRARIES GenEvent GaudiAlgLib LHCbKernel GeneratorsLib ${HEPMC3_LIBRARIES})

add_dependencies(LbPGuns HepMC3Ext)

gaudi_install_headers(LbPGuns)

