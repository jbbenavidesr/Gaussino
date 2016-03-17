################################################################################
# Package: GaussRD
################################################################################
gaudi_subdir(GaussRD v6r0p1)

gaudi_depends_on_subdirs(Det/DetDescCnv)

gaudi_add_module(GaussRD
                 src/*.cpp)

gaudi_install_headers(GaussRD)
