################################################################################
# Package: GenBeam
################################################################################
gaudi_subdir(GenBeam v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/GenEvent
                         Gen/GenInterfaces
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel)

gaudi_add_module(GenBeam
                 src/*.cpp
                 INCLUDE_DIRS Gen/GenInterfaces
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel)
