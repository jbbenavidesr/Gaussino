gaudi_subdir(EDMConv v7r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiKernel
                         Defaults)

find_package(HepMC)


gaudi_add_module(EDMConv
                 src/*.cpp
                 INCLUDE_DIRS HepMC Defaults
                 LINK_LIBRARIES GenEvent MCEvent GaudiAlgLib GaudiKernel HepMC)
