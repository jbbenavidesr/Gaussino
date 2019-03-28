################################################################################
# Package: GenInterface
################################################################################
gaudi_subdir(GenInterfaces v1r0)

gaudi_depends_on_subdirs(NewRnd)

find_package(HepMC CONFIG)

gaudi_install_headers(
  INCLUDE_DIRS NewRnd ${HEPMC_INCLUDE_DIR}
  GenInterfaces)

gaudi_install_headers(
  Event)
