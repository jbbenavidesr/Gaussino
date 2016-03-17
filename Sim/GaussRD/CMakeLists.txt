################################################################################
# Package: GaussRD
################################################################################
gaudi_subdir(GaussRD v6r0p1)

gaudi_depends_on_subdirs(Det/DetDescCnv
                         Sim/GaussTools)

gaudi_add_module(GaussRD
                 src/*.cpp
                 LINK_LIBRARIES GaussToolsLib)

gaudi_install_headers(GaussRD)
