gaudi_subdir(EDMConv v7r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiKernel)


gaudi_add_module(EDMConv
                 src/*.cpp
                 LINK_LIBRARIES GenEvent MCEvent GaudiAlgLib GaudiKernel)
