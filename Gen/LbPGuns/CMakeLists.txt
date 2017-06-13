################################################################################
# Package: LbPGuns
################################################################################
gaudi_subdir(LbPGuns v6r1)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/GenEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Gen/Generators)

gaudi_add_module(LbPGuns
                 src/*.cpp
                 LINK_LIBRARIES DetDescLib GenEvent GaudiAlgLib LHCbKernel GeneratorsLib)

gaudi_install_headers(LbPGuns)

