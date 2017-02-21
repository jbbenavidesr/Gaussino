################################################################################
# Package: GaussRedecay
################################################################################
gaudi_subdir(GaussRedecay v1r0)

gaudi_depends_on_subdirs(Gen/Generators
                         Event/MCEvent
                         Event/GenEvent
                         Event/PhysEvent
                         Sim/GiGa)

gaudi_add_module(GaussRedecay
                 src/*.cpp
                 LINK_LIBRARIES GenEvent MCEvent PhysEvent GiGaLib)

gaudi_install_headers(GaussRedecay)
