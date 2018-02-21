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
                         Gen/Generators)

gaudi_add_module(LbPGuns
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces
                 LINK_LIBRARIES GenEvent GaudiAlgLib LHCbKernel GeneratorsLib)

gaudi_install_headers(LbPGuns)

