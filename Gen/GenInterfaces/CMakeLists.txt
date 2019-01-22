################################################################################
# Package: GenInterface
################################################################################
gaudi_subdir(GenInterfaces v1r0)

gaudi_depends_on_subdirs(NewRnd)

gaudi_install_headers(
  INCLUDE_DIRS NewRnd
  GenInterfaces)
