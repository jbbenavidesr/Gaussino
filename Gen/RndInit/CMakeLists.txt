################################################################################
# Package: GaussAlgs
################################################################################
gaudi_subdir(RndInit v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/GenEvent
                         #FIXME: For platform and kernel streaming, I think ?!?!
                         Kernel/LHCbKernel
                         Defaults)

gaudi_add_module(RndInit
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GenEvent LHCbKernel)
